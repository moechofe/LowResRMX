import 'dart:developer';
import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter/painting.dart';

import 'package:path/path.dart' as p;
import 'package:image/image.dart' as img;

import 'package:lowresrmx/data/preference.dart';
// import 'package:lowresrmx/data/firestore.dart';

enum MyLibrarySort {
  name,
  oldest,
  newest,
}

/// A class to manage the library of programs.
/// Listening to this class will notify when program are created, renamed or deleted.
class MyLibrary extends ChangeNotifier {
  static final MyLibrary _singleton = MyLibrary._internal();

  factory MyLibrary() {
    return _singleton;
  }

  MyLibrary._internal();

  static String extension = ".rmx";

  static Future<Directory> getLibraryDir() async {
    final String libraryPath = await MyPreference.getProgramDirectory();
    final Directory libraryDir = Directory(libraryPath);
    if (!await libraryDir.exists()) await libraryDir.create(recursive: true);
    return libraryDir;
  }

  static Future<File> getCodeFile(String programName) async {
    final documentDir = await getLibraryDir();
    final codePath = p.join(documentDir.path, "$programName$extension");
    return File(codePath);
  }

  static Future<File> getThumbFile(String programName) async {
    final documentDir = await getLibraryDir();
    final codePath = p.join(documentDir.path, "$programName.png");
    return File(codePath);
  }

  static Future<File> createProgram() async {
    final Directory libraryDir = await getLibraryDir();
    String programPath = p.join(libraryDir.path, "unnamed$extension");
    File programFile = File(programPath);
    int counter = -1;
    while (await programFile.exists()) {
      counter += 1;
      programPath = p.join(libraryDir.path, "unnamed $counter$extension");
      programFile = File(programPath);
    }
    await programFile.create();

    // MyFirestore.createProgram(p.basenameWithoutExtension(programPath));

    MyLibrary().notifyListeners();
    return programFile;
  }

  static Future<void> renameProgram(String programName, String nameName) async {
    final Directory libraryDir = await getLibraryDir();
		// Prevent bad characters in the name
		nameName = nameName.replaceAll(RegExp(r'[^\w\s_]+'), '');
		if (nameName.isEmpty) { nameName = "unnamed"; }
		// Search for a unique name
		final String endingDigits = nameName.replaceAll(RegExp(r'(.*)\d+$'), '');
		int counter = 0;
		if (endingDigits.isNotEmpty) { counter = int.tryParse(endingDigits) ?? 1; }
		File candidateFile = File(p.join(libraryDir.path, "$nameName$extension"));
		while(await candidateFile.exists()) {
			nameName += " $counter";
			counter += 1;
			candidateFile = File(p.join(libraryDir.path, "$nameName$extension"));
		}
    final String programPath =
        p.join(libraryDir.path, "$programName$extension");
    final File programFile = File(p.join(libraryDir.path, programPath));
    if (await programFile.exists()) {
      await programFile.rename(p.join(libraryDir.path, "$nameName$extension"));
    }
    final String thumbPath = p.join(libraryDir.path, "$programName.png");
    final File thumbFile = File(p.join(libraryDir.path, thumbPath));
    FileImage(thumbFile).evict();
    if (await thumbFile.exists()) {
      await thumbFile.rename(p.join(libraryDir.path, "$nameName.png"));
    }
    MyLibrary().notifyListeners();
  }

  static Future<void> deleteProgram(String programName) async {
    final Directory libraryDir = await getLibraryDir();

    final String programPath =
        p.join(libraryDir.path, "$programName$extension");
    final File programFile = File(p.join(libraryDir.path, programPath));
    if (await programFile.exists()) {
      await programFile.delete();
    }
    final String thumbPath = p.join(libraryDir.path, "$programName.png");
    final File thumbFile = File(p.join(libraryDir.path, thumbPath));
    FileImage(thumbFile).evict();
    if (await thumbFile.exists()) {
      await thumbFile.delete();
    }
    MyLibrary().notifyListeners();
  }

  static List<File> sortByName(List<File> list) {
    list.sort((a, b) => a.path.compareTo(b.path));
    return list;
  }

  static List<File> sortByOldest(List<File> list) {
    list.sort((a, b) => a.lastModifiedSync().compareTo(b.lastModifiedSync()));
    return list;
  }

  static List<File> sortByNewest(List<File> list) {
    list.sort((a, b) => b.lastModifiedSync().compareTo(a.lastModifiedSync()));
    return list;
  }

  static Future<List<String>> buildList(MyLibrarySort sort) async {
		log("MyLibrary.buildList()");
		await MyLibrary.createDataDiskIfNotExists();

    final Directory libraryDir = await getLibraryDir();

    try {
      List<File> fileList = await libraryDir
          .list()
          .where((entry) => p.extension(entry.path) == extension)
					.where((entry) => p.basename(entry.path) != ".dataDisk$extension")
          .asyncMap((entry) => entry as File)
          .toList();

      switch (sort) {
        case MyLibrarySort.name:
          fileList = await compute(sortByName, fileList);
        case MyLibrarySort.oldest:
          fileList = await compute(sortByOldest, fileList);
        case MyLibrarySort.newest:
          fileList = await compute(sortByNewest, fileList);
      }

      return fileList
          .map((file) => p.basenameWithoutExtension(file.path))
          .toList();
    } catch (e) {
      log("Error: $e");

      return [];
    }
  }

  // TODO: convert to UPPERCASE using settings
  static Future<String> readCode(String programName) async {
    final File codeFile = await getCodeFile(programName);
    return codeFile.readAsString();
  }

  static Future<void> writeCode(String programName, String code) async {
    final File codeFile = await getCodeFile(programName);
    codeFile.writeAsString(code);
  }

  static Future<FileImage> readThumbnail(String programName) async {
    // This a async
    final File thumbFile = await getThumbFile(programName);
    // This is not async
    return FileImage(thumbFile);
  }

  static Future<void> writeThumbnail(String programName, img.Image png) async {
    final File thumbFile = await getThumbFile(programName);
    await FileImage(thumbFile).evict();
    img.encodePngFile(thumbFile.path, png);
    MyLibrary().notifyListeners();
  }

	static Future<void> createDataDiskIfNotExists() async {
    final Directory libraryDir = await getLibraryDir();
    final String programPath =
        p.join(libraryDir.path, ".dataDisk$extension");
    final File programFile = File(p.join(libraryDir.path, programPath));
    if (await programFile.exists() == false) {
			await programFile.create();
		}
	}
}

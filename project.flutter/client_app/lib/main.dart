import 'dart:developer' show log;

import 'package:flutter/material.dart';

import 'package:provider/provider.dart';

import 'package:lowresrmx/core/runtime.dart';
import 'package:lowresrmx/data/library.dart';
import 'package:lowresrmx/page/library_page.dart';
import 'package:lowresrmx/theme.dart';


// TODO: delete the preference if the file is removed
// TODO: rename the preference if the file is renamed


// import 'dart:math' as math;
// import 'dart:async';
// import 'dart:typed_data';

// import 'package:mp_audio_stream/mp_audio_stream.dart';

// void main() async {
//   final audioStream = getAudioStream();

//   //default init params: {int bufferMilliSec = 3000,
//   //                      int waitingBufferMilliSec = 100,
//   //                      int channels = 1,
//   //                      int sampleRate = 44100}
//   audioStream.init(channels: 2); //Call this from Flutter's State.initState() method

//   const rate = 44100;
//   const freqL = 440;
//   const freqR = 660;
//   const dur = 10;
//   Float32List samples = Float32List(rate);

//   audioStream.resume(); //For the web, call this after user interaction

//   for (var t = 0; t < dur; t++) {
//     int pos = 0;
//     for (var i = 0; i < rate; i++) {
//       samples[pos++] = math.sin(2 * math.pi * ((i * freqL) % rate) / rate);
//       samples[pos++] = math.sin(2 * math.pi * ((i * freqR) % rate) / rate);
//       if (pos == samples.length) {
//         pos = 0;
//         audioStream.push(samples);
//       }
//     }
//     if (t > 0) {
//       await Future.delayed(const Duration(seconds: 1));
//     }
//   }

//   audioStream.uninit(); //Call this from Flutter's State.dispose()
// }




void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  final Runtime coreRuntime;

  MyApp({super.key}) : coreRuntime = Runtime();

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  @override
  void initState() {
    super.initState();
    widget.coreRuntime.initState();
  }

  @override
  void dispose() {
    super.dispose();
    widget.coreRuntime.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final MyTheme theme = MyTheme(Theme.of(context).textTheme);
    log("MyApp.build() Not good if called multiple times.");
    return MultiProvider(
        providers: [
          ChangeNotifierProvider<Runtime>(
            create: (_) => widget.coreRuntime,
          ),
          ChangeNotifierProvider<MyLibrary>(create: (_) => MyLibrary()),
        ],
        child: MaterialApp(
          title: 'LowResRMX',
          theme: theme.light(),
          darkTheme: theme.dark(),
          highContrastTheme: theme.lightHighContrast(),
          highContrastDarkTheme: theme.darkHighContrast(),
          initialRoute: '/',
          routes: {
            '/': (context) => const MyLibraryPage(),
          },
        ));
  }
}

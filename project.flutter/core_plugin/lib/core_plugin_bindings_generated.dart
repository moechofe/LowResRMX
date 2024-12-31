// ignore_for_file: always_specify_types
// ignore_for_file: camel_case_types
// ignore_for_file: non_constant_identifier_names

// AUTO GENERATED FILE, DO NOT EDIT.
//
// Generated by `package:ffigen`.
// ignore_for_file: type=lint
import 'dart:ffi' as ffi;

/// Bindings for `src/core_plugin.h`.
///
/// Regenerate bindings with `dart run ffigen --config ffigen.yaml`.
///
class CorePluginBindings {
  /// Holds the symbol lookup function.
  final ffi.Pointer<T> Function<T extends ffi.NativeType>(String symbolName)
      _lookup;

  /// The symbols are looked up in [dynamicLibrary].
  CorePluginBindings(ffi.DynamicLibrary dynamicLibrary)
      : _lookup = dynamicLibrary.lookup;

  /// The symbols are looked up with [lookup].
  CorePluginBindings.fromLookup(
      ffi.Pointer<T> Function<T extends ffi.NativeType>(String symbolName)
          lookup)
      : _lookup = lookup;

  void runnerInit(
    ffi.Pointer<Runner> arg0,
  ) {
    return _runnerInit(
      arg0,
    );
  }

  late final _runnerInitPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Pointer<Runner>)>>(
          'runnerInit');
  late final _runnerInit =
      _runnerInitPtr.asFunction<void Function(ffi.Pointer<Runner>)>();

  void runnerDeinit(
    ffi.Pointer<Runner> arg0,
  ) {
    return _runnerDeinit(
      arg0,
    );
  }

  late final _runnerDeinitPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Pointer<Runner>)>>(
          'runnerDeinit');
  late final _runnerDeinit =
      _runnerDeinitPtr.asFunction<void Function(ffi.Pointer<Runner>)>();

  CoreError runnerCompileProgram(
    ffi.Pointer<Runner> arg0,
    ffi.Pointer<ffi.Char> arg1,
  ) {
    return _runnerCompileProgram(
      arg0,
      arg1,
    );
  }

  late final _runnerCompileProgramPtr = _lookup<
      ffi.NativeFunction<
          CoreError Function(ffi.Pointer<Runner>,
              ffi.Pointer<ffi.Char>)>>('runnerCompileProgram');
  late final _runnerCompileProgram = _runnerCompileProgramPtr.asFunction<
      CoreError Function(ffi.Pointer<Runner>, ffi.Pointer<ffi.Char>)>();

  ffi.Pointer<ffi.Char> runnerGetError(
    ffi.Pointer<Runner> arg0,
    int arg1,
  ) {
    return _runnerGetError(
      arg0,
      arg1,
    );
  }

  late final _runnerGetErrorPtr = _lookup<
      ffi.NativeFunction<
          ffi.Pointer<ffi.Char> Function(
              ffi.Pointer<Runner>, ffi.Int32)>>('runnerGetError');
  late final _runnerGetError = _runnerGetErrorPtr
      .asFunction<ffi.Pointer<ffi.Char> Function(ffi.Pointer<Runner>, int)>();

  void runnerStart(
    ffi.Pointer<Runner> arg0,
    int scondsSincePowerOn,
    ffi.Pointer<ffi.Char> originalDataDisk,
    int originalDataDiskSize,
  ) {
    return _runnerStart(
      arg0,
      scondsSincePowerOn,
      originalDataDisk,
      originalDataDiskSize,
    );
  }

  late final _runnerStartPtr = _lookup<
      ffi.NativeFunction<
          ffi.Void Function(ffi.Pointer<Runner>, ffi.Int, ffi.Pointer<ffi.Char>,
              ffi.Size)>>('runnerStart');
  late final _runnerStart = _runnerStartPtr.asFunction<
      void Function(ffi.Pointer<Runner>, int, ffi.Pointer<ffi.Char>, int)>();

  CoreError runnerUpdate(
    ffi.Pointer<Runner> arg0,
    ffi.Pointer<Input> arg1,
  ) {
    return _runnerUpdate(
      arg0,
      arg1,
    );
  }

  late final _runnerUpdatePtr = _lookup<
      ffi.NativeFunction<
          CoreError Function(
              ffi.Pointer<Runner>, ffi.Pointer<Input>)>>('runnerUpdate');
  late final _runnerUpdate = _runnerUpdatePtr.asFunction<
      CoreError Function(ffi.Pointer<Runner>, ffi.Pointer<Input>)>();

  void runnerRender(
    ffi.Pointer<Runner> arg0,
    ffi.Pointer<ffi.Void> arg1,
  ) {
    return _runnerRender(
      arg0,
      arg1,
    );
  }

  late final _runnerRenderPtr = _lookup<
      ffi.NativeFunction<
          ffi.Void Function(
              ffi.Pointer<Runner>, ffi.Pointer<ffi.Void>)>>('runnerRender');
  late final _runnerRender = _runnerRenderPtr
      .asFunction<void Function(ffi.Pointer<Runner>, ffi.Pointer<ffi.Void>)>();

  void runnerTrace(
    ffi.Pointer<Runner> arg0,
    bool arg1,
  ) {
    return _runnerTrace(
      arg0,
      arg1,
    );
  }

  late final _runnerTracePtr = _lookup<
          ffi.NativeFunction<ffi.Void Function(ffi.Pointer<Runner>, ffi.Bool)>>(
      'runnerTrace');
  late final _runnerTrace =
      _runnerTracePtr.asFunction<void Function(ffi.Pointer<Runner>, bool)>();

  int runnerGetSymbolCount(
    ffi.Pointer<Runner> arg0,
  ) {
    return _runnerGetSymbolCount(
      arg0,
    );
  }

  late final _runnerGetSymbolCountPtr =
      _lookup<ffi.NativeFunction<ffi.Int Function(ffi.Pointer<Runner>)>>(
          'runnerGetSymbolCount');
  late final _runnerGetSymbolCount =
      _runnerGetSymbolCountPtr.asFunction<int Function(ffi.Pointer<Runner>)>();

  ffi.Pointer<ffi.Char> runnerGetSymbolName(
    ffi.Pointer<Runner> arg0,
    int arg1,
  ) {
    return _runnerGetSymbolName(
      arg0,
      arg1,
    );
  }

  late final _runnerGetSymbolNamePtr = _lookup<
      ffi.NativeFunction<
          ffi.Pointer<ffi.Char> Function(
              ffi.Pointer<Runner>, ffi.Int)>>('runnerGetSymbolName');
  late final _runnerGetSymbolName = _runnerGetSymbolNamePtr
      .asFunction<ffi.Pointer<ffi.Char> Function(ffi.Pointer<Runner>, int)>();

  int runnerGetSymbolPosition(
    ffi.Pointer<Runner> arg0,
    int arg1,
  ) {
    return _runnerGetSymbolPosition(
      arg0,
      arg1,
    );
  }

  late final _runnerGetSymbolPositionPtr = _lookup<
          ffi.NativeFunction<ffi.Int Function(ffi.Pointer<Runner>, ffi.Int)>>(
      'runnerGetSymbolPosition');
  late final _runnerGetSymbolPosition = _runnerGetSymbolPositionPtr
      .asFunction<int Function(ffi.Pointer<Runner>, int)>();

  void inputKeyDown(
    ffi.Pointer<Input> arg0,
    int arg1,
  ) {
    return _inputKeyDown(
      arg0,
      arg1,
    );
  }

  late final _inputKeyDownPtr = _lookup<
          ffi.NativeFunction<ffi.Void Function(ffi.Pointer<Input>, ffi.Int)>>(
      'inputKeyDown');
  late final _inputKeyDown =
      _inputKeyDownPtr.asFunction<void Function(ffi.Pointer<Input>, int)>();
}

/// @brief Hold stuff to control the core.
final class Runner extends ffi.Struct {
  /// @brief Pointer to the core.
  external ffi.Pointer<Core> core;

  /// @brief Pointer to the core's delegate. I am totally not using the delegate has it was designed. I never manage to make it work. On the c side some Flutter pointer wasn't readable.
  external CoreDelegate delegate;

  /// @brief Used to report the error when a program is running. It is filled by a delegate
  external CoreError runningError;

  /// @brief Holding the data disk, memory is own by the core.
  external ffi.Pointer<ffi.Char> dataDisk;

  /// @brief Size of the data disk.
  @ffi.Int()
  external int dataDiskSize;

  /// @brief Used to know if the data disk should be saved on Flutter side.
  @ffi.Bool()
  external bool shouldSaveDisk;

  /// @brief Used to know if the keyboard should be opened on Flutter side.
  @ffi.Bool()
  external bool shouldOpenKeyboard;

  /// @brief Used to tell when the client app enable the input mode. It should allow to open the keyboard using tap.
  @ffi.Bool()
  external bool shouldEnableInputMode;
}

/// TODO: EMITTER_MAX and SPAWNER_MAX should be the same, right?
final class Core extends ffi.Struct {
  external ffi.Pointer<Machine> machine;

  external ffi.Pointer<MachineInternals> machineInternals;

  external ffi.Pointer<Interpreter> interpreter;

  external ffi.Pointer<DiskDrive> diskDrive;

  external ffi.Pointer<Overlay> overlay;

  external ffi.Pointer<CoreDelegate> delegate;
}

/// 128Kibi
final class Machine extends ffi.Opaque {}

final class MachineInternals extends ffi.Opaque {}

final class Interpreter extends ffi.Opaque {}

final class DiskDrive extends ffi.Struct {
  external DataManager dataManager;
}

final class DataManager extends ffi.Struct {
  @ffi.Array.multi([16])
  external ffi.Array<DataEntry> entries;

  external ffi.Pointer<ffi.Uint8> data;

  external ffi.Pointer<ffi.Char> diskSourceCode;
}

final class DataEntry extends ffi.Struct {
  @ffi.Array.multi([32])
  external ffi.Array<ffi.Char> comment;

  @ffi.Int()
  external int start;

  @ffi.Int()
  external int length;
}

final class Overlay extends ffi.Opaque {}

final class CoreDelegate extends ffi.Struct {
  external ffi.Pointer<ffi.Void> context;

  /// Called on error
  external ffi.Pointer<
          ffi.NativeFunction<
              ffi.Void Function(
                  ffi.Pointer<ffi.Void> context, CoreError coreError)>>
      interpreterDidFail;

  /// Returns true if the disk is ready, false if not. In case of not, core_diskLoaded must be called when ready.
  external ffi.Pointer<
      ffi.NativeFunction<
          ffi.Bool Function(ffi.Pointer<ffi.Void> context,
              ffi.Pointer<DataManager> diskDataManager)>> diskDriveWillAccess;

  /// Called when a disk data entry was saved
  external ffi.Pointer<
      ffi.NativeFunction<
          ffi.Void Function(ffi.Pointer<ffi.Void> context,
              ffi.Pointer<DataManager> diskDataManager)>> diskDriveDidSave;

  /// Called when a disk data entry was tried to be saved, but the disk is full
  external ffi.Pointer<
      ffi.NativeFunction<
          ffi.Void Function(ffi.Pointer<ffi.Void> context,
              ffi.Pointer<DataManager> diskDataManager)>> diskDriveIsFull;

  /// Called when keyboard or gamepad settings changed
  external ffi.Pointer<
          ffi.NativeFunction<
              ffi.Void Function(
                  ffi.Pointer<ffi.Void> context, ControlsInfo controlsInfo)>>
      controlsDidChange;

  /// Called when persistent RAM will be accessed the first time
  external ffi.Pointer<
      ffi.NativeFunction<
          ffi.Void Function(
              ffi.Pointer<ffi.Void> context,
              ffi.Pointer<ffi.Uint8> destination,
              ffi.Int size)>> persistentRamWillAccess;

  /// Called when persistent RAM should be saved
  external ffi.Pointer<
      ffi.NativeFunction<
          ffi.Void Function(
              ffi.Pointer<ffi.Void> context,
              ffi.Pointer<ffi.Uint8> data,
              ffi.Int size)>> persistentRamDidChange;
}

final class CoreError extends ffi.Struct {
  @ffi.Int32()
  external int code;

  @ffi.Int()
  external int sourcePosition;
}

abstract class ErrorCode {
  static const int ErrorNone = 0;
  static const int ErrorCouldNotOpenProgram = 1;
  static const int ErrorTooManyTokens = 2;
  static const int ErrorRomIsFull = 3;
  static const int ErrorIndexAlreadyDefined = 4;
  static const int ErrorUnterminatedString = 5;
  static const int ErrorUnexpectedCharacter = 6;
  static const int ErrorReservedKeyword = 7;
  static const int ErrorSyntax = 8;
  static const int ErrorSymbolNameTooLong = 9;
  static const int ErrorTooManySymbols = 10;
  static const int ErrorTypeMismatch = 11;
  static const int ErrorOutOfMemory = 12;
  static const int ErrorElseWithoutIf = 13;
  static const int ErrorEndIfWithoutIf = 14;
  static const int ErrorExpectedCommand = 15;
  static const int ErrorNextWithoutFor = 16;
  static const int ErrorLoopWithoutDo = 17;
  static const int ErrorUntilWithoutRepeat = 18;
  static const int ErrorWendWithoutWhile = 19;
  static const int ErrorLabelAlreadyDefined = 20;
  static const int ErrorTooManyLabels = 21;
  static const int ErrorExpectedLabel = 22;
  static const int ErrorUndefinedLabel = 23;
  static const int ErrorArrayNotDimensionized = 24;
  static const int ErrorArrayAlreadyDimensionized = 25;
  static const int ErrorVariableAlreadyUsed = 26;
  static const int ErrorIndexOutOfBounds = 27;
  static const int ErrorWrongNumberOfDimensions = 28;
  static const int ErrorInvalidParameter = 29;
  static const int ErrorReturnWithoutGosub = 30;
  static const int ErrorStackOverflow = 31;
  static const int ErrorOutOfData = 32;
  static const int ErrorIllegalMemoryAccess = 33;
  static const int ErrorTooManyCPUCyclesInInterrupt = 34;
  static const int ErrorNotAllowedInInterrupt = 35;
  static const int ErrorIfWithoutEndIf = 36;
  static const int ErrorForWithoutNext = 37;
  static const int ErrorDoWithoutLoop = 38;
  static const int ErrorRepeatWithoutUntil = 39;
  static const int ErrorWhileWithoutWend = 40;
  static const int ErrorExitNotInsideLoop = 41;
  static const int ErrorDirectoryNotLoaded = 42;
  static const int ErrorDivisionByZero = 43;
  static const int ErrorVariableNotInitialized = 44;
  static const int ErrorArrayVariableWithoutIndex = 45;
  static const int ErrorEndSubWithoutSub = 46;
  static const int ErrorSubWithoutEndSub = 47;
  static const int ErrorSubCannotBeNested = 48;
  static const int ErrorUndefinedSubprogram = 49;
  static const int ErrorExpectedSubprogramName = 50;
  static const int ErrorArgumentCountMismatch = 51;
  static const int ErrorSubAlreadyDefined = 52;
  static const int ErrorTooManySubprograms = 53;
  static const int ErrorSharedOutsideOfASubprogram = 54;
  static const int ErrorGlobalInsideOfASubprogram = 55;
  static const int ErrorExitSubOutsideOfASubprogram = 56;
  static const int ErrorAutomaticPauseNotDisabled = 57;
  static const int ErrorNotAllowedOutsideOfInterrupt = 58;
  static const int ErrorUserDeviceDiskFull = 59;
  static const int ErrorMax = 60;
}

final class ControlsInfo extends ffi.Struct {
  @ffi.Int32()
  external int keyboardMode;

  @ffi.Bool()
  external bool isAudioEnabled;

  @ffi.Bool()
  external bool isInputState;
}

abstract class KeyboardMode {
  static const int KeyboardModeOff = 0;
  static const int KeyboardModeOn = 1;
  static const int KeyboardModeOptional = 2;
}

typedef Input = CoreInput;

final class CoreInput extends ffi.Struct {
  /// For SHOWN and SAFE
  @ffi.Int()
  external int width;

  @ffi.Int()
  external int height;

  @ffi.Int()
  external int left;

  @ffi.Int()
  external int top;

  @ffi.Int()
  external int right;

  @ffi.Int()
  external int bottom;

  @ffi.Bool()
  external bool pause;

  @ffi.Float()
  external double touchX;

  @ffi.Float()
  external double touchY;

  @ffi.Bool()
  external bool touch;

  @ffi.Char()
  external int key;

  @ffi.Bool()
  external bool out_hasUsedInput;
}

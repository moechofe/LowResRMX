//
// Copyright 2017 Timo Kloss
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef error_h
#define error_h

#include <stdio.h>

enum ErrorCode {
    ErrorNone,

    ErrorCouldNotOpenProgram,
    ErrorTooManyTokens,
    ErrorRomIsFull,
    ErrorIndexAlreadyDefined,
    ErrorUnterminatedString,
    ErrorUnexpectedCharacter,
    ErrorReservedKeyword,
    ErrorSyntax,
    ErrorSymbolNameTooLong,
    ErrorTooManySymbols,
    ErrorTypeMismatch,
    ErrorOutOfMemory,
    ErrorElseWithoutIf,
    ErrorEndIfWithoutIf,
    ErrorExpectedCommand,
    ErrorNextWithoutFor,
    ErrorLoopWithoutDo,
    ErrorUntilWithoutRepeat,
    ErrorWendWithoutWhile,
    ErrorLabelAlreadyDefined,
    ErrorTooManyLabels,
    ErrorExpectedLabel,
    ErrorUndefinedLabel,
    ErrorArrayNotDimensionized,
    ErrorArrayAlreadyDimensionized,
    ErrorVariableAlreadyUsed,
    ErrorIndexOutOfBounds,
    ErrorWrongNumberOfDimensions,
    ErrorInvalidParameter,
    ErrorReturnWithoutGosub,
    ErrorStackOverflow,
    ErrorOutOfData,
    ErrorIllegalMemoryAccess,
    ErrorTooManyCPUCyclesInInterrupt,
    ErrorNotAllowedInInterrupt,
    ErrorIfWithoutEndIf,
    ErrorForWithoutNext,
    ErrorDoWithoutLoop,
    ErrorRepeatWithoutUntil,
    ErrorWhileWithoutWend,
    ErrorExitNotInsideLoop,
    ErrorDirectoryNotLoaded,
    ErrorDivisionByZero,
    ErrorVariableNotInitialized,
    ErrorArrayVariableWithoutIndex,
    ErrorEndSubWithoutSub,
    ErrorSubWithoutEndSub,
    ErrorSubCannotBeNested,
    ErrorUndefinedSubprogram,
    ErrorExpectedSubprogramName,
    ErrorArgumentCountMismatch,
    ErrorSubAlreadyDefined,
    ErrorTooManySubprograms,
    ErrorSharedOutsideOfASubprogram,
    ErrorGlobalInsideOfASubprogram,
    ErrorExitSubOutsideOfASubprogram,
    ErrorAutomaticPauseNotDisabled,
    ErrorNotAllowedOutsideOfInterrupt,
		ErrorUserDeviceDiskFull,

		ErrorMax
};

struct CoreError {
    enum ErrorCode code;
    int sourcePosition;
};

const char *err_getString(enum ErrorCode errorCode);
struct CoreError err_makeCoreError(enum ErrorCode code, int sourcePosition);
struct CoreError err_noCoreError(void);

#endif /* error_h */

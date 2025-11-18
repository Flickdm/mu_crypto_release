/** @file
  Base Debug Library implementation using OneCrypto dependency injection.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseCryptCrtLib.h>
#include <Library/PrintLib.h>
#include <Private/OneCryptoDependencySupport.h>

/**
  Prints a debug message to the debug output device if the specified error level is enabled.

  @param  ErrorLevel  The error level of the debug message.
  @param  Format      Format string for the debug message to print.
  @param  ...         Variable argument list whose contents are accessed 
                      based on the format string specified by Format.

**/
VOID
EFIAPI
DebugPrint (
  IN  UINTN        ErrorLevel,
  IN  CONST CHAR8  *Format,
  ...
  )
{
  ONE_CRYPTO_DEPENDENCIES  *Deps;
  VA_LIST                  Marker;
  CHAR8                    Buffer[256];

  Deps = GetCryptoDependencies ();
  if ((Deps != NULL) && (Deps->DebugPrint != NULL)) {
    VA_START (Marker, Format);
    // Format the string first using AsciiVSPrint
    AsciiVSPrint (Buffer, sizeof(Buffer), Format, Marker);
    VA_END (Marker);
    // Now call DebugPrint with the formatted string (no more varargs)
    Deps->DebugPrint (ErrorLevel, "%a", Buffer);
  }
}

/**
  Prints an assert message containing a filename, line number, and description.
  This may be followed by a breakpoint or a dead loop.

  @param  FileName     The pointer to the name of the source file that generated the assert condition.
  @param  LineNumber   The line number in the source file that generated the assert condition
  @param  Description  The pointer to the description of the assert condition.

**/
VOID
EFIAPI
DebugAssert (
  IN CONST CHAR8  *FileName,
  IN UINTN        LineNumber,
  IN CONST CHAR8  *Description
  )
{
  ONE_CRYPTO_DEPENDENCIES  *Deps;

  Deps = GetCryptoDependencies ();
  if ((Deps != NULL) && (Deps->ASSERT != NULL)) {
    // OneCrypto's ASSERT takes a boolean expression, but DebugAssert is called when assertion fails
    // So we pass FALSE to trigger the assert
    Deps->ASSERT (FALSE);
  } else {
    // Fallback: spin loop
    while (TRUE) {
    }
  }
}

/**
  Fills a target buffer with PcdDebugClearMemoryValue, and returns the target buffer.

  @param  Buffer  The pointer to the target buffer to be filled with PcdDebugClearMemoryValue.
  @param  Length  The number of bytes in Buffer to fill with zeros PcdDebugClearMemoryValue.

  @return Buffer  The pointer to the target buffer filled with PcdDebugClearMemoryValue.

**/
VOID *
EFIAPI
DebugClearMemory (
  OUT VOID   *Buffer,
  IN  UINTN  Length
  )
{
  // Not implemented for OneCrypto
  return Buffer;
}

/**
  Returns TRUE if ASSERT() macros are enabled.

  @retval  TRUE    The DEBUG_PROPERTY_DEBUG_ASSERT_ENABLED bit of PcdDebugProperyMask is set.
  @retval  FALSE   The DEBUG_PROPERTY_DEBUG_ASSERT_ENABLED bit of PcdDebugProperyMask is clear.

**/
BOOLEAN
EFIAPI
DebugAssertEnabled (
  VOID
  )
{
  return TRUE;
}

/**
  Returns TRUE if DEBUG() macros are enabled.

  @retval  TRUE    The DEBUG_PROPERTY_DEBUG_PRINT_ENABLED bit of PcdDebugProperyMask is set.
  @retval  FALSE   The DEBUG_PROPERTY_DEBUG_PRINT_ENABLED bit of PcdDebugProperyMask is clear.

**/
BOOLEAN
EFIAPI
DebugPrintEnabled (
  VOID
  )
{
  return TRUE;
}

/**
  Returns TRUE if DEBUG_CODE() macros are enabled.

  @retval  TRUE    The DEBUG_PROPERTY_DEBUG_CODE_ENABLED bit of PcdDebugProperyMask is set.
  @retval  FALSE   The DEBUG_PROPERTY_DEBUG_CODE_ENABLED bit of PcdDebugProperyMask is clear.

**/
BOOLEAN
EFIAPI
DebugCodeEnabled (
  VOID
  )
{
  return FALSE;
}

/**
  Returns TRUE if DEBUG_CLEAR_MEMORY() macro is enabled.

  @retval  TRUE    The DEBUG_PROPERTY_CLEAR_MEMORY_ENABLED bit of PcdDebugProperyMask is set.
  @retval  FALSE   The DEBUG_PROPERTY_CLEAR_MEMORY_ENABLED bit of PcdDebugProperyMask is clear.

**/
BOOLEAN
EFIAPI
DebugClearMemoryEnabled (
  VOID
  )
{
  return FALSE;
}

/**
  Returns TRUE if any one of the bit is set both in ErrorLevel and PcdFixedDebugPrintErrorLevel.

  @param  ErrorLevel    The error level of the debug message.

  @retval TRUE          The error level is enabled.
  @retval FALSE         The error level is disabled.

**/
BOOLEAN
EFIAPI
DebugPrintLevelEnabled (
  IN  CONST UINTN  ErrorLevel
  )
{
  return TRUE;
}

/** @file
  Minimal BasePrintLib implementation

  This library provides minimal BasePrintLib functionality for OneCryptoPkg.
  Functions will be implemented as needed.

  This is a simplified custom implementation optimized for minimal dependencies.
  Unlike MU_BASECORE/MdePkg/Library/BasePrintLib which uses the complex
  BasePrintLibSPrintMarker internal engine, this implementation provides basic
  format string parsing for common use cases (%s, %d, %u, %x, %X, %%).

  Function signatures match those in MU_BASECORE/MdePkg/Library/BasePrintLib/PrintLib.c:
  - AsciiSPrint: Wrapper function (matches BasePrintLib pattern)
  - AsciiVSPrint: Simplified custom implementation (not based on BasePrintLibSPrintMarker)

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/PrintLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <stdarg.h>

//
// Helper functions and constants
//
STATIC CONST CHAR8 mHexStr[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

/**
  Convert a value to an ASCII string.

  This is a custom implementation for the minimal library.

  @param  Buffer      The output buffer.
  @param  BufferSize  The size of the buffer.
  @param  Flags       The conversion flags.
  @param  Value       The value to convert.
  @param  Width       The field width.

  @return The length of the converted string.

**/
UINTN
AsciiValueToString (
  OUT CHAR8   *Buffer,
  IN  UINTN   BufferSize,
  IN  UINTN   Flags,
  IN  UINT64  Value,
  IN  UINTN   Width
  );

/**
  Divides a 64-bit unsigned integer by a 32-bit unsigned integer and generates 
  a 64-bit unsigned result.

  This is a simple wrapper around the division operator.

  @param  Dividend  A 64-bit unsigned value.
  @param  Divisor   A 32-bit unsigned value.

  @return Dividend / Divisor.

**/
UINT64
DivU64x32 (
  IN      UINT64  Dividend,
  IN      UINT32  Divisor
  );

/**
  Produces a Null-terminated ASCII string in an output buffer based on a Null-terminated
  ASCII format string and variable argument list.

  Function signature matches MU_BASECORE/MdePkg/Library/BasePrintLib/PrintLib.c.
  This is a standard wrapper that calls AsciiVSPrint.

  @param  StartOfBuffer   A pointer to the output buffer for the produced Null-terminated 
                          ASCII string.
  @param  BufferSize      The size, in bytes, of the output buffer specified by StartOfBuffer.
  @param  FormatString    A Null-terminated ASCII format string.
  @param  ...             Variable argument list whose contents are accessed based on the 
                          format string specified by FormatString.

  @return The number of ASCII characters in the produced output buffer not including the
          Null-terminator.

**/
UINTN
EFIAPI
AsciiSPrint (
  OUT CHAR8        *StartOfBuffer,
  IN  UINTN        BufferSize,
  IN  CONST CHAR8  *FormatString,
  ...
  )
{
  VA_LIST  Marker;
  UINTN    NumberOfPrinted;

  VA_START (Marker, FormatString);
  NumberOfPrinted = AsciiVSPrint (StartOfBuffer, BufferSize, FormatString, Marker);
  VA_END (Marker);
  return NumberOfPrinted;
}

/**
  Produces a Null-terminated ASCII string in an output buffer based on a Null-terminated
  ASCII format string and a VA_LIST argument list.

  Function signature matches MU_BASECORE/MdePkg/Library/BasePrintLib/PrintLib.c.
  This is a simplified custom implementation that provides basic format specifier support
  (%s, %d, %u, %x, %X, %%) without the full complexity of BasePrintLibSPrintMarker.

  @param  StartOfBuffer   A pointer to the output buffer for the produced Null-terminated 
                          ASCII string.
  @param  BufferSize      The size, in bytes, of the output buffer specified by StartOfBuffer.
  @param  FormatString    A Null-terminated ASCII format string.
  @param  Marker          VA_LIST marker for the variable argument list.

  @return The number of ASCII characters in the produced output buffer not including the
          Null-terminator.

**/
UINTN
EFIAPI
AsciiVSPrint (
  OUT CHAR8        *StartOfBuffer,
  IN  UINTN        BufferSize,
  IN  CONST CHAR8  *FormatString,
  IN  VA_LIST      Marker
  )
{
  CHAR8        *Buffer;
  CONST CHAR8  *Format;
  UINTN        Index;
  UINTN        Count;
  UINT64       Value;
  CHAR8        *String;
  UINTN        Length;
  CHAR8        ValueBuffer[22];  // Enough for MAX_UINT64 in decimal
  BOOLEAN      Done;
  BOOLEAN      ArgumentParsed;

  if ((BufferSize == 0) || (StartOfBuffer == NULL) || (FormatString == NULL)) {
    return 0;
  }

  Buffer = StartOfBuffer;
  Format = FormatString;
  Count  = 0;

  for (Index = 0; ((*Format != '\0') && (Index < BufferSize - 1)); ) {
    if (*Format != '%') {
      Buffer[Index++] = *Format++;
      Count++;
    } else {
      //
      // Parse Flags and Width
      //
      Format++;
      Done           = FALSE;
      ArgumentParsed = FALSE;
      
      //
      // Simple format specifier parsing - only handle basic cases
      //
      while (!Done) {
        switch (*Format) {
          case '%':
            Buffer[Index++] = '%';
            Format++;
            Count++;
            Done           = TRUE;
            ArgumentParsed = TRUE;
            break;

          case 's':
            String = VA_ARG (Marker, CHAR8 *);
            if (String == NULL) {
              String = "(null)";
            }
            Length = AsciiStrLen (String);
            if (Index + Length >= BufferSize) {
              Length = BufferSize - Index - 1;
            }
            for (UINTN StringIndex = 0; StringIndex < Length; StringIndex++) {
              Buffer[Index++] = String[StringIndex];
            }
            Count         += Length;
            Format++;
            Done           = TRUE;
            ArgumentParsed = TRUE;
            break;

          case 'd':
          case 'u':
            Value = VA_ARG (Marker, UINTN);
            AsciiValueToString (ValueBuffer, sizeof (ValueBuffer), 0, Value, 22);
            Length = AsciiStrLen (ValueBuffer);
            if (Index + Length >= BufferSize) {
              Length = BufferSize - Index - 1;
            }
            for (UINTN StringIndex = 0; StringIndex < Length; StringIndex++) {
              Buffer[Index++] = ValueBuffer[StringIndex];
            }
            Count         += Length;
            Format++;
            Done           = TRUE;
            ArgumentParsed = TRUE;
            break;

          case 'x':
          case 'X':
            Value = VA_ARG (Marker, UINTN);
            AsciiValueToString (ValueBuffer, sizeof (ValueBuffer), 1, Value, 16);
            Length = AsciiStrLen (ValueBuffer);
            if (Index + Length >= BufferSize) {
              Length = BufferSize - Index - 1;
            }
            for (UINTN StringIndex = 0; StringIndex < Length; StringIndex++) {
              Buffer[Index++] = ValueBuffer[StringIndex];
            }
            Count         += Length;
            Format++;
            Done           = TRUE;
            ArgumentParsed = TRUE;
            break;

          default:
            // Unsupported format specifier, just copy it
            if (Index < BufferSize - 1) {
              Buffer[Index++] = '%';
              Count++;
            }
            if ((Index < BufferSize - 1) && (*Format != '\0')) {
              Buffer[Index++] = *Format++;
              Count++;
            }
            Done = TRUE;
            break;
        }
      }

      if (!ArgumentParsed) {
        //
        // Skip the unsupported format specifier argument
        //
        VA_ARG (Marker, UINTN);
      }
    }
  }

  Buffer[Index] = '\0';
  return Count;
}

/**
  Convert a value to an ASCII string.

  This is a custom implementation for the minimal library.
  Supports decimal (radix 10) and hexadecimal (radix 16) conversion.

  @param  Buffer      The output buffer.
  @param  BufferSize  The size of the buffer.
  @param  Flags       The conversion flags (bit 0: 0=decimal, 1=hexadecimal).
  @param  Value       The value to convert.
  @param  Width       The field width.

  @return The length of the converted string.

**/
UINTN
AsciiValueToString (
  OUT CHAR8   *Buffer,
  IN  UINTN   BufferSize,
  IN  UINTN   Flags,
  IN  UINT64  Value,
  IN  UINTN   Width
  )
{
  CHAR8  TempBuffer[22];  // Enough for MAX_UINT64
  CHAR8  *TempPtr;
  UINTN  Count;
  UINTN  Index;
  CHAR8  Prefix;
  UINTN  Radix;
  UINT64 Remainder;

  TempPtr = TempBuffer;
  Count   = 0;
  Radix   = ((Flags & 1) != 0) ? 16 : 10;

  if (Width == 0) {
    Width = 1;
  }

  if (Width > 22) {
    Width = 22;
  }

  //
  // Convert the value to ASCII in reverse order
  //
  *TempPtr = '\0';
  do {
    Remainder = DivU64x32 (Value, (UINT32)Radix);
    Prefix  = mHexStr[Value - (Remainder * Radix)];
    Value = Remainder;
    *(++TempPtr) = Prefix;
    Count++;
    if (Count == sizeof (TempBuffer) - 1) {
      break;
    }
  } while (Value != 0);

  //
  // Copy the converted string to the output buffer
  //
  for (Index = 0; (Index < BufferSize - 1) && (TempPtr != TempBuffer); Index++) {
    Buffer[Index] = *(TempPtr--);
  }

  Buffer[Index] = '\0';
  return Index;
}

/**
  Divides a 64-bit unsigned integer by a 32-bit unsigned integer and generates 
  a 64-bit unsigned result.

  This is a simple wrapper around the division operator for compatibility.

  @param  Dividend  A 64-bit unsigned value.
  @param  Divisor   A 32-bit unsigned value.

  @return Dividend / Divisor.

**/
UINT64
DivU64x32 (
  IN      UINT64  Dividend,
  IN      UINT32  Divisor
  )
{
  return Dividend / Divisor;
}

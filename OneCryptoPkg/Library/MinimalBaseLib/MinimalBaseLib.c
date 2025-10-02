/** @file
  Minimal BaseLib implementation

  This library provides minimal BaseLib functionality for OneCryptoPkg.
  Functions will be implemented as needed.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

//
// Constants
//
#ifndef SIZE_MAX
#define SIZE_MAX  ((UINTN) -1)
#endif

#define RSIZE_MAX  (SIZE_MAX >> 1)

//
// PCD defaults - we don't want to depend on PcdLib for minimal implementation
//
#define MAX_ASCII_STRING_LENGTH  0x8000000

//
// Helper function declarations
//
BOOLEAN
InternalAsciiIsSpace (
  IN      CHAR8  Char
  );

BOOLEAN
InternalAsciiIsDecimalDigitCharacter (
  IN      CHAR8  Char
  );

BOOLEAN
InternalSafeStringNoAsciiStrOverlap (
  IN CHAR8   *Destination,
  IN UINTN   DestMax,
  IN CHAR8   *Source,
  IN UINTN   SourceLen
  );

//
// Macro for safe string constraint checking
//
#define SAFE_STRING_CONSTRAINT_CHECK(Expression, Status)  \
  do { \
    ASSERT (Expression); \
    if (!(Expression)) { \
      return Status; \
    } \
  } while (FALSE)

/**
  Returns the length of a Null-terminated ASCII string.

  @param  String  A pointer to a Null-terminated ASCII string.

  @return The length of String.

**/
UINTN
EFIAPI
AsciiStrLen (
  IN      CONST CHAR8  *String
  )
{
  UINTN  Length;

  ASSERT (String != NULL);

  for (Length = 0; *String != '\0'; String++, Length++) {
    //
    // If MAX_ASCII_STRING_LENGTH is not zero,
    // length should not more than MAX_ASCII_STRING_LENGTH
    //
    if (MAX_ASCII_STRING_LENGTH != 0) {
      ASSERT (Length < MAX_ASCII_STRING_LENGTH);
    }
  }

  return Length;
}

/**
  Returns the size of a Null-terminated ASCII string in bytes, including the
  Null terminator.

  @param  String  A pointer to a Null-terminated ASCII string.

  @return The size of String.

**/
UINTN
EFIAPI
AsciiStrSize (
  IN      CONST CHAR8  *String
  )
{
  return (AsciiStrLen (String) + 1) * sizeof (*String);
}

/**
  Returns the length of a Null-terminated ASCII string.

  @param  String   A pointer to a Null-terminated ASCII string.
  @param  MaxSize  The maximum number of Destination ASCII
                   char, including terminating null char.

  @retval 0        If String is NULL.
  @retval MaxSize  If there is no null character in the first MaxSize characters of String.
  @return The number of characters that precede the terminating null character.

**/
UINTN
EFIAPI
AsciiStrnLenS (
  IN CONST CHAR8  *String,
  IN UINTN        MaxSize
  )
{
  UINTN  Length;

  //
  // If String is a null pointer or MaxSize is 0, then the AsciiStrnLenS function returns zero.
  //
  if ((String == NULL) || (MaxSize == 0)) {
    return 0;
  }

  //
  // Otherwise, the AsciiStrnLenS function returns the number of characters that precede the
  // terminating null character. If there is no null character in the first MaxSize characters of
  // String then AsciiStrnLenS returns MaxSize. At most the first MaxSize characters of String shall
  // be accessed by AsciiStrnLenS.
  //
  Length = 0;
  while (String[Length] != 0) {
    if (Length >= MaxSize - 1) {
      return MaxSize;
    }

    Length++;
  }

  return Length;
}

/**
  Compares two Null-terminated ASCII strings, and returns the difference
  between the first mismatched ASCII characters.

  @param  FirstString   A pointer to a Null-terminated ASCII string.
  @param  SecondString  A pointer to a Null-terminated ASCII string.

  @retval ==0      FirstString is identical to SecondString.
  @retval !=0      FirstString is not identical to SecondString.

**/
INTN
EFIAPI
AsciiStrCmp (
  IN      CONST CHAR8  *FirstString,
  IN      CONST CHAR8  *SecondString
  )
{
  //
  // ASSERT both strings are less long than PcdMaximumAsciiStringLength
  //
  ASSERT (AsciiStrSize (FirstString));
  ASSERT (AsciiStrSize (SecondString));

  while ((*FirstString != '\0') && (*FirstString == *SecondString)) {
    FirstString++;
    SecondString++;
  }

  return *FirstString - *SecondString;
}

/**
  Compares two Null-terminated ASCII strings with maximum lengths, and returns
  the difference between the first mismatched ASCII characters.

  @param  FirstString   A pointer to a Null-terminated ASCII string.
  @param  SecondString  A pointer to a Null-terminated ASCII string.
  @param  Length        The maximum number of ASCII characters for compare.

  @retval ==0       FirstString is identical to SecondString.
  @retval !=0       FirstString is not identical to SecondString.

**/
INTN
EFIAPI
AsciiStrnCmp (
  IN      CONST CHAR8  *FirstString,
  IN      CONST CHAR8  *SecondString,
  IN      UINTN        Length
  )
{
  if (Length == 0) {
    return 0;
  }

  //
  // ASSERT both strings are less long than PcdMaximumAsciiStringLength.
  // Length tests are performed inside AsciiStrLen().
  //
  ASSERT (AsciiStrSize (FirstString) != 0);
  ASSERT (AsciiStrSize (SecondString) != 0);

  while ((*FirstString != '\0') &&
         (*SecondString != '\0') &&
         (*FirstString == *SecondString) &&
         (Length > 1))
  {
    FirstString++;
    SecondString++;
    Length--;
  }

  return *FirstString - *SecondString;
}

/**
  Copies the string pointed to by Source (including the terminating null char)
  to the array pointed to by Destination.

  @param  Destination              A pointer to a Null-terminated Ascii string.
  @param  DestMax                  The maximum number of Destination Ascii
                                   char, including terminating null char.
  @param  Source                   A pointer to a Null-terminated Ascii string.

  @retval RETURN_SUCCESS           String is copied.
  @retval RETURN_BUFFER_TOO_SMALL  If DestMax is NOT greater than StrLen(Source).
  @retval RETURN_INVALID_PARAMETER If Destination is NULL.
                                   If Source is NULL.
                                   If PcdMaximumAsciiStringLength is not zero,
                                    and DestMax is greater than 
                                    PcdMaximumAsciiStringLength.
                                   If DestMax is 0.

**/
RETURN_STATUS
EFIAPI
AsciiStrCpyS (
  OUT CHAR8        *Destination,
  IN  UINTN        DestMax,
  IN  CONST CHAR8  *Source
  )
{
  UINTN  SourceLen;

  //
  // 1. Neither Destination nor Source shall be a null pointer.
  //
  SAFE_STRING_CONSTRAINT_CHECK ((Destination != NULL), RETURN_INVALID_PARAMETER);
  SAFE_STRING_CONSTRAINT_CHECK ((Source != NULL), RETURN_INVALID_PARAMETER);

  //
  // 2. DestMax shall not be greater than RSIZE_MAX.
  //
  if (RSIZE_MAX != 0) {
    SAFE_STRING_CONSTRAINT_CHECK ((DestMax <= RSIZE_MAX), RETURN_INVALID_PARAMETER);
  }

  //
  // 3. DestMax shall not equal zero.
  //
  SAFE_STRING_CONSTRAINT_CHECK ((DestMax != 0), RETURN_INVALID_PARAMETER);

  //
  // 4. DestMax shall be greater than AsciiStrnLenS(Source, DestMax).
  //
  SourceLen = AsciiStrnLenS (Source, DestMax);
  SAFE_STRING_CONSTRAINT_CHECK ((DestMax > SourceLen), RETURN_BUFFER_TOO_SMALL);

  //
  // The AsciiStrCpyS function copies the string pointed to by Source (including the terminating
  // null character) into the array pointed to by Destination.
  //
  while (*Source != 0) {
    *(Destination++) = *(Source++);
  }

  *Destination = 0;

  return RETURN_SUCCESS;
}

/**
  Copies not more than Length successive char from the string pointed to by
  Source to the array pointed to by Destination. If no null char is copied from
  Source, then Destination[Length] is always set to null.

  @param  Destination              A pointer to a Null-terminated Ascii string.
  @param  DestMax                  The maximum number of Destination Ascii
                                   char, including terminating null char.
  @param  Source                   A pointer to a Null-terminated Ascii string.
  @param  Length                   The maximum number of characters to copy from source.

  @retval RETURN_SUCCESS           String is copied.
  @retval RETURN_BUFFER_TOO_SMALL  If DestMax is NOT greater than 
                                   MIN(StrLen(Source), Length).
  @retval RETURN_INVALID_PARAMETER If Destination is NULL.
                                   If Source is NULL.
                                   If PcdMaximumAsciiStringLength is not zero,
                                    and DestMax is greater than 
                                    PcdMaximumAsciiStringLength.
                                   If DestMax is 0.

**/
RETURN_STATUS
EFIAPI
AsciiStrnCpyS (
  OUT CHAR8        *Destination,
  IN  UINTN        DestMax,
  IN  CONST CHAR8  *Source,
  IN  UINTN        Length
  )
{
  UINTN  SourceLen;

  //
  // 1. Neither Destination nor Source shall be a null pointer.
  //
  SAFE_STRING_CONSTRAINT_CHECK ((Destination != NULL), RETURN_INVALID_PARAMETER);
  SAFE_STRING_CONSTRAINT_CHECK ((Source != NULL), RETURN_INVALID_PARAMETER);

  //
  // 2. Neither DestMax nor Length shall be greater than RSIZE_MAX
  //
  if (RSIZE_MAX != 0) {
    SAFE_STRING_CONSTRAINT_CHECK ((DestMax <= RSIZE_MAX), RETURN_INVALID_PARAMETER);
    SAFE_STRING_CONSTRAINT_CHECK ((Length <= RSIZE_MAX), RETURN_INVALID_PARAMETER);
  }

  //
  // 3. DestMax shall not equal zero.
  //
  SAFE_STRING_CONSTRAINT_CHECK ((DestMax != 0), RETURN_INVALID_PARAMETER);

  //
  // 4. If Length is not less than DestMax, then DestMax shall be greater than AsciiStrnLenS(Source, DestMax).
  //
  SourceLen = AsciiStrnLenS (Source, (DestMax < Length) ? DestMax : Length);
  if (Length >= DestMax) {
    SAFE_STRING_CONSTRAINT_CHECK ((DestMax > SourceLen), RETURN_BUFFER_TOO_SMALL);
  }

  //
  // 5. Copying shall not take place between objects that overlap.
  //
  SAFE_STRING_CONSTRAINT_CHECK (InternalSafeStringNoAsciiStrOverlap (Destination, DestMax, (CHAR8 *)Source, SourceLen + 1), RETURN_ACCESS_DENIED);

  //
  // The AsciiStrnCpyS function copies not more than Length successive characters (characters that
  // follow a null character are not copied) from the array pointed to by Source to the array
  // pointed to by Destination. If no null character was copied from Source, then Destination[Length] is set to a null
  // character.
  //
  while ((SourceLen > 0) && (*Source != 0)) {
    *(Destination++) = *(Source++);
    SourceLen--;
    Length--;
  }

  *Destination = 0;
  return RETURN_SUCCESS;
}

/**
  Returns the first occurrence of a Null-terminated ASCII sub-string
  in a Null-terminated ASCII string.

  @param  String          A pointer to a Null-terminated ASCII string.
  @param  SearchString    A pointer to a Null-terminated ASCII string to search for.

  @retval NULL            If the SearchString does not appear in String.
  @retval others          If the SearchString appears in String.

**/
CHAR8 *
EFIAPI
AsciiStrStr (
  IN      CONST CHAR8  *String,
  IN      CONST CHAR8  *SearchString
  )
{
  CONST CHAR8  *FirstMatch;
  CONST CHAR8  *SearchStringTmp;

  //
  // ASSERT both strings are less long than PcdMaximumAsciiStringLength
  //
  ASSERT (AsciiStrSize (String) != 0);
  ASSERT (AsciiStrSize (SearchString) != 0);

  if (*SearchString == '\0') {
    return (CHAR8 *)String;
  }

  while (*String != '\0') {
    SearchStringTmp = SearchString;
    FirstMatch      = String;

    while ((*String == *SearchStringTmp) &&
           (*String != '\0'))
    {
      String++;
      SearchStringTmp++;
    }

    if (*SearchStringTmp == '\0') {
      return (CHAR8 *)FirstMatch;
    }

    if (*String == '\0') {
      return NULL;
    }

    String = FirstMatch + 1;
  }

  return NULL;
}

/**
  Convert a Null-terminated ASCII decimal string to a value of type UINTN.

  @param  String          A pointer to a Null-terminated ASCII string.

  @retval The value translated from String.

**/
UINTN
EFIAPI
AsciiStrDecimalToUintn (
  IN      CONST CHAR8  *String
  )
{
  UINTN     Result;
  RETURN_STATUS  Status;

  Status = AsciiStrDecimalToUintnS (String, (CHAR8 **)NULL, &Result);
  return Result;
}

/**
  Convert a Null-terminated ASCII decimal string to a value of type UINTN.

  @param  String              A pointer to a Null-terminated ASCII string.
  @param  EndPointer          If EndPointer is not NULL, a pointer to the character that
                              stopped the scan is stored at the location pointed to
                              by EndPointer. If conversion stopped because the
                              beginning of the string does not contain a valid decimal
                              number, the value of *EndPointer is String.
  @param  Data                A pointer to the converted value.

  @retval RETURN_SUCCESS           Value is translated from String.
  @retval RETURN_INVALID_PARAMETER If String is NULL.
                                   If Data is NULL.
                                   If PcdMaximumAsciiStringLength is not zero,
                                   and String contains more than
                                   PcdMaximumAsciiStringLength ASCII
                                   characters, not including the
                                   Null-terminator.
  @retval RETURN_UNSUPPORTED       If the number represented by String exceeds
                                   the range defined by UINTN.

**/
RETURN_STATUS
EFIAPI
AsciiStrDecimalToUintnS (
  IN  CONST CHAR8  *String,
  OUT CHAR8        **EndPointer  OPTIONAL,
  OUT UINTN        *Data
  )
{
  ASSERT (String != NULL);
  ASSERT (Data != NULL);

  if (String == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  if (Data == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  if (EndPointer != NULL) {
    *EndPointer = (CHAR8 *)String;
  }

  // Ignore leading white space characters
  while (InternalAsciiIsSpace (*String)) {
    String++;
  }

  //
  // Ignore leading zeros after the spaces
  //
  while (*String == '0') {
    String++;
  }

  *Data = 0;

  while (InternalAsciiIsDecimalDigitCharacter (*String)) {
    //
    // If the number represented by String overflows according to the range
    // defined by UINTN, then MAX_UINTN is stored in *Data and
    // RETURN_UNSUPPORTED is returned.
    //
    if (*Data > ((MAX_UINTN - (*String - '0')) / 10)) {
      *Data = MAX_UINTN;
      if (EndPointer != NULL) {
        *EndPointer = (CHAR8 *)String;
      }

      return RETURN_UNSUPPORTED;
    }

    *Data = *Data * 10 + (*String - '0');
    String++;
  }

  if (EndPointer != NULL) {
    *EndPointer = (CHAR8 *)String;
  }

  return RETURN_SUCCESS;
}

//
// Helper functions
//

BOOLEAN
InternalAsciiIsSpace (
  IN      CHAR8  Char
  )
{
  return (BOOLEAN)(Char == ' ' || Char == '\t' || Char == '\v' || Char == '\f' || Char == '\r' || Char == '\n');
}

BOOLEAN
InternalAsciiIsDecimalDigitCharacter (
  IN      CHAR8  Char
  )
{
  return (BOOLEAN)(Char >= '0' && Char <= '9');
}

BOOLEAN
InternalSafeStringNoAsciiStrOverlap (
  IN CHAR8   *Destination,
  IN UINTN   DestMax,
  IN CHAR8   *Source,
  IN UINTN   SourceLen
  )
{
  UINTN  DestLen;
  UINTN  DestEnd;
  UINTN  SourceEnd;

  DestLen = AsciiStrnLenS (Destination, DestMax);
  DestEnd = (UINTN)Destination + DestLen;
  SourceEnd = (UINTN)Source + SourceLen;
  
  if (((UINTN)Source >= (UINTN)Destination) && ((UINTN)Source < DestEnd)) {
    return FALSE;
  }
  if (((UINTN)Destination >= (UINTN)Source) && ((UINTN)Destination < SourceEnd)) {
    return FALSE;
  }
  return TRUE;
}
/** @file
  64-bit Math Functions for MinimalBaseLib

  This file provides 64-bit signed integer division function for OneCryptoPkg.
  Implementation is based on MU_BASECORE/MdePkg/Library/BaseLib/Math64.c

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

/**
  Divides a 64-bit signed integer by a 64-bit signed integer and generates a
  64-bit signed result and an optional 64-bit signed remainder.

  This function divides the 64-bit signed value Dividend by the 64-bit signed
  value Divisor and generates a 64-bit signed quotient. If Remainder is not
  NULL, then the 64-bit signed remainder is returned in Remainder. This
  function returns the 64-bit signed quotient.

  It is the caller's responsibility to not call this function with a Divisor of 0.
  If Divisor is 0, then the result is unpredictable.

  @param  Dividend  A 64-bit signed value.
  @param  Divisor   A 64-bit signed value.
  @param  Remainder A pointer to a 64-bit signed value. This parameter is
                    optional and may be NULL.

  @return Dividend / Divisor.

**/
INT64
EFIAPI
InternalMathDivRemS64x64 (
  IN      INT64  Dividend,
  IN      INT64  Divisor,
  OUT     INT64  *Remainder  OPTIONAL
  )
{
  if (Remainder != NULL) {
    *Remainder = Dividend % Divisor;
  }

  return Dividend / Divisor;
}

/**
  Divides a 64-bit signed integer by a 64-bit signed integer and generates a
  64-bit signed result and a 64-bit signed remainder.

  This function divides the 64-bit signed value Dividend by the 64-bit signed
  value Divisor and generates a 64-bit signed quotient. The 64-bit signed
  remainder is returned in Remainder. This function returns the 64-bit signed
  quotient.

  It is the caller's responsibility to not call this function with a Divisor of 0.
  If Divisor is 0, then the result is unpredictable.

  @param  Dividend  A 64-bit signed value.
  @param  Divisor   A 64-bit signed value.
  @param  Remainder A pointer to a 64-bit signed value.

  @return Dividend / Divisor.

**/
INT64
EFIAPI
DivS64x64Remainder (
  IN      INT64  Dividend,
  IN      INT64  Divisor,
  OUT     INT64  *Remainder
  )
{
  ASSERT (Divisor != 0);
  return InternalMathDivRemS64x64 (Dividend, Divisor, Remainder);
}

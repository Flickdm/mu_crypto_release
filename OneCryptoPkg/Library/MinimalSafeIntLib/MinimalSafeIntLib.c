/** @file
  Minimal Safe Integer Library implementation.
  
  Provides simple safe math functions to avoid external dependencies
  in the OneCrypto binary.

Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/MinimalSafeIntLib.h>

/**
  Safely add two UINTN values.

  @param[in]  Augend   First value to add.
  @param[in]  Addend   Second value to add.
  @param[out] Result   Pointer to store the result.

  @retval RETURN_SUCCESS           The addition was successful.
  @retval RETURN_BUFFER_TOO_SMALL  Overflow would occur.
**/
RETURN_STATUS
EFIAPI
SafeUintnAdd (
  IN  UINTN  Augend,
  IN  UINTN  Addend,
  OUT UINTN  *Result
  )
{
  if (((UINTN)(~0)) - Augend < Addend) {
    return RETURN_BUFFER_TOO_SMALL;
  }

  *Result = Augend + Addend;
  return RETURN_SUCCESS;
}

/**
  Safely multiply two UINTN values.

  @param[in]  Multiplicand  First value to multiply.
  @param[in]  Multiplier    Second value to multiply.
  @param[out] Result        Pointer to store the result.

  @retval RETURN_SUCCESS           The multiplication was successful.
  @retval RETURN_BUFFER_TOO_SMALL  Overflow would occur.
**/
RETURN_STATUS
EFIAPI
SafeUintnMult (
  IN  UINTN  Multiplicand,
  IN  UINTN  Multiplier,
  OUT UINTN  *Result
  )
{
  if (Multiplicand > 0 && (((UINTN)(~0)) / Multiplicand) < Multiplier) {
    return RETURN_BUFFER_TOO_SMALL;
  }

  *Result = Multiplicand * Multiplier;
  return RETURN_SUCCESS;
}

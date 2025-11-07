/** @file
  One Crypto CRT Library that helps perform dependency injection 

  Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef ONE_CRYPTO_DEBUG_LIBRARY_H_
#define ONE_CRYPTO_DEBUG_LIBRARY_H_

#include <Private/OneCryptoCrtLibSupport.h>

extern ONE_CRYPTO_DEPENDENCIES  *gOneCryptoDepends;

#define ASSERT(Expression) \
  do { \
    if (gOneCryptoDepends != NULL && gOneCryptoDepends->ASSERT != NULL) { \
      gOneCryptoDepends->ASSERT(Expression); \
    } else { \
      /* Fallback: minimal self-contained assert - spin loop on failure */ \
      while (!(Expression)) { \
        /* Spin loop for assert failure */ \
      } \
    } \
  } while (0)

#define ASSERT_EFI_ERROR(StatusParameter) \
  do { \
    if (gOneCryptoDepends != NULL && gOneCryptoDepends->ASSERT != NULL) { \
      gOneCryptoDepends->ASSERT(!EFI_ERROR(StatusParameter)); \
    } else { \
      /* Fallback: minimal self-contained EFI error assert */ \
      while (EFI_ERROR(StatusParameter)) { \
        /* Spin loop for EFI error */ \
      } \
    } \
  } while (0)

// Allows the Crypto Binary to use UEFI style DEBUG print
#define DEBUG_ERROR    0x80000000
#define DEBUG_WARN     0x40000000
#define DEBUG_INFO     0x20000000
#define DEBUG_VERBOSE  0x10000000

/**
  Macro to print debug messages.

  This macro checks if the global shared dependencies and the DebugPrint function
  pointer within it are not NULL. If both are valid, it calls the DebugPrint function
  with the provided arguments.

  @param[in] Args  The arguments to pass to the DebugPrint function.

  @note This macro does nothing if gOneCryptoDepends or gOneCryptoDepends->DebugPrint is NULL.

  @since 1.0.0
  @ingroup External
**/
#define DEBUG(Args)                                                       \
  do                                                                      \
  {                                                                       \
    if (gOneCryptoDepends != NULL && gOneCryptoDepends->DebugPrint != NULL) \
    {                                                                     \
      gOneCryptoDepends->DebugPrint Args;                                 \
    }                                                                     \
  } while (0)

/**
  Returns TRUE if DEBUG_CODE() macros are enabled.

  This function returns TRUE if the DEBUG_PROPERTY_DEBUG_CODE_ENABLED bit of
  PcdDebugProperyMask is set.  Otherwise FALSE is returned.

  @retval  TRUE    The DEBUG_PROPERTY_DEBUG_CODE_ENABLED bit of PcdDebugProperyMask is set.
  @retval  FALSE   The DEBUG_PROPERTY_DEBUG_CODE_ENABLED bit of PcdDebugProperyMask is clear.

**/
BOOLEAN
EFIAPI
DebugCodeEnabled (
  VOID
  );

/**
  Macro that marks the beginning of debug source code.

  If the DEBUG_PROPERTY_DEBUG_CODE_ENABLED bit of PcdDebugProperyMask is set,
  then this macro marks the beginning of source code that is included in a module.
  Otherwise, the source lines between DEBUG_CODE_BEGIN() and DEBUG_CODE_END()
  are not included in a module.

**/
#define DEBUG_CODE_BEGIN()  do { if (DebugCodeEnabled ()) { UINT8  __DebugCodeLocal

/**
  The macro that marks the end of debug source code.

  If the DEBUG_PROPERTY_DEBUG_CODE_ENABLED bit of PcdDebugProperyMask is set,
  then this macro marks the end of source code that is included in a module.
  Otherwise, the source lines between DEBUG_CODE_BEGIN() and DEBUG_CODE_END()
  are not included in a module.

**/
#define DEBUG_CODE_END()  __DebugCodeLocal = 0; __DebugCodeLocal++; } } while (FALSE)

#endif // ONE_CRYPTO_DEBUG_LIBRARY_H_

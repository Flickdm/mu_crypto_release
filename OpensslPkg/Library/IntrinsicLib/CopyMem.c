/** @file
  Intrinsic Memory Routines Wrapper Implementation for OpenSSL-based
  Cryptographic Library.

  This file wraps memcpy() to call UEFI's CopyMem() function.
  
  Note: While this includes <Library/BaseMemoryLib.h> from MdePkg, the actual
  implementation of CopyMem() is provided by MinimalBaseMemoryLib as configured
  in the DSC file's LibraryClasses section. This ensures IntrinsicLib only links
  the minimal memory functions required for OpenSSL.

Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseMemoryLib.h>

#if defined (__clang__) && !defined (__APPLE__)

/* Copies bytes between buffers */
static __attribute__ ((__used__))
void *
__memcpy (
  void          *dest,
  const void    *src,
  unsigned int  count
  )
{
  return CopyMem (dest, src, (UINTN)count);
}

__attribute__ ((__alias__ ("__memcpy")))
void *
memcpy (
  void          *dest,
  const void    *src,
  unsigned int  count
  );

#else
/* Copies bytes between buffers */
void *
memcpy (
  void          *dest,
  const void    *src,
  unsigned int  count
  )
{
  return CopyMem (dest, src, (UINTN)count);
}

#endif

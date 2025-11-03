/** @file
  Minimal BaseMemoryLib implementation

  This library provides minimal BaseMemoryLib functionality for OneCryptoPkg.
  Functions will be implemented as needed.

  Function implementations are based on MU_BASECORE/MdePkg/Library/BaseMemoryLib:
  - CopyMem: Based on InternalMemCopyMem from CopyMem.c
  - CompareMem: Based on InternalMemCompareMem from CompareMemWrapper.c
  - SetMem: Based on InternalMemSetMem from SetMem.c
  - ScanMem8: Based on InternalMemScanMem8 from ScanMem8Wrapper.c

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

/**
  Copies a source buffer to a destination buffer, and returns the destination buffer.

  This implementation is based on InternalMemCopyMem from
  MU_BASECORE/MdePkg/Library/BaseMemoryLib/CopyMem.c

  @param  DestinationBuffer   A pointer to the destination buffer of the memory copy.
  @param  SourceBuffer        A pointer to the source buffer of the memory copy.
  @param  Length              The number of bytes to copy from SourceBuffer to DestinationBuffer.

  @return DestinationBuffer.

**/
VOID *
EFIAPI
CopyMem (
  OUT VOID       *DestinationBuffer,
  IN CONST VOID  *SourceBuffer,
  IN UINTN       Length
  )
{
  volatile UINT8   *Destination8;
  CONST UINT8      *Source8;
  volatile UINT32  *Destination32;
  CONST UINT32     *Source32;
  volatile UINT64  *Destination64;
  CONST UINT64     *Source64;
  UINTN            Alignment;

  if (Length == 0) {
    return DestinationBuffer;
  }

  ASSERT ((Length - 1) <= (MAX_ADDRESS - (UINTN)DestinationBuffer));
  ASSERT ((Length - 1) <= (MAX_ADDRESS - (UINTN)SourceBuffer));

  if (DestinationBuffer == SourceBuffer) {
    return DestinationBuffer;
  }

  if ((((UINTN)DestinationBuffer & 0x7) == 0) && (((UINTN)SourceBuffer & 0x7) == 0) && (Length >= 8)) {
    if (SourceBuffer > DestinationBuffer) {
      Destination64 = (UINT64 *)DestinationBuffer;
      Source64      = (CONST UINT64 *)SourceBuffer;
      while (Length >= 8) {
        *(Destination64++) = *(Source64++);
        Length            -= 8;
      }

      // Finish if there are still some bytes to copy
      Destination8 = (UINT8 *)Destination64;
      Source8      = (CONST UINT8 *)Source64;
      while (Length-- != 0) {
        *(Destination8++) = *(Source8++);
      }
    } else if (SourceBuffer < DestinationBuffer) {
      Destination64 = (UINT64 *)((UINTN)DestinationBuffer + Length);
      Source64      = (CONST UINT64 *)((UINTN)SourceBuffer + Length);

      Alignment = Length & 0x7;
      if (Alignment != 0) {
        Destination8 = (UINT8 *)Destination64;
        Source8      = (CONST UINT8 *)Source64;

        while (Alignment-- != 0) {
          *(--Destination8) = *(--Source8);
          --Length;
        }

        Destination64 = (UINT64 *)Destination8;
        Source64      = (CONST UINT64 *)Source8;
      }

      while (Length > 0) {
        *(--Destination64) = *(--Source64);
        Length            -= 8;
      }
    }
  } else {
    if ((((UINTN)DestinationBuffer & 0x3) == 0) && (((UINTN)SourceBuffer & 0x3) == 0) && (Length >= 4)) {
      if (SourceBuffer > DestinationBuffer) {
        Destination32 = (UINT32 *)DestinationBuffer;
        Source32      = (CONST UINT32 *)SourceBuffer;
        while (Length >= 4) {
          *(Destination32++) = *(Source32++);
          Length            -= 4;
        }

        Destination8 = (UINT8 *)Destination32;
        Source8      = (CONST UINT8 *)Source32;
        while (Length-- != 0) {
          *(Destination8++) = *(Source8++);
        }
      } else if (SourceBuffer < DestinationBuffer) {
        Destination32 = (UINT32 *)((UINTN)DestinationBuffer + Length);
        Source32      = (CONST UINT32 *)((UINTN)SourceBuffer + Length);

        Alignment = Length & 0x3;
        if (Alignment != 0) {
          Destination8 = (UINT8 *)Destination32;
          Source8      = (CONST UINT8 *)Source32;

          while (Alignment-- != 0) {
            *(--Destination8) = *(--Source8);
            --Length;
          }

          Destination32 = (UINT32 *)Destination8;
          Source32      = (CONST UINT32 *)Source8;
        }

        while (Length > 0) {
          *(--Destination32) = *(--Source32);
          Length            -= 4;
        }
      }
    } else {
      if (SourceBuffer > DestinationBuffer) {
        Destination8 = (UINT8 *)DestinationBuffer;
        Source8      = (CONST UINT8 *)SourceBuffer;
        while (Length-- != 0) {
          *(Destination8++) = *(Source8++);
        }
      } else if (SourceBuffer < DestinationBuffer) {
        Destination8 = (UINT8 *)DestinationBuffer + (Length - 1);
        Source8      = (CONST UINT8 *)SourceBuffer + (Length - 1);
        while (Length-- != 0) {
          *(Destination8--) = *(Source8--);
        }
      }
    }
  }

  return DestinationBuffer;
}

/**
  Compares the contents of two buffers.

  This implementation is based on InternalMemCompareMem from
  MU_BASECORE/MdePkg/Library/BaseMemoryLib/CompareMemWrapper.c

  @param  DestinationBuffer A pointer to the destination buffer to compare.
  @param  SourceBuffer      A pointer to the source buffer to compare.
  @param  Length            The number of bytes to compare.

  @return 0                 All Length bytes of the two buffers are identical.
  @retval Non-zero          The first mismatched byte in SourceBuffer subtracted from the first
                            mismatched byte in DestinationBuffer.

**/
INTN
EFIAPI
CompareMem (
  IN CONST VOID  *DestinationBuffer,
  IN CONST VOID  *SourceBuffer,
  IN UINTN       Length
  )
{
  CONST UINT8  *DestinationBytes;
  CONST UINT8  *SourceBytes;

  if ((Length == 0) || (DestinationBuffer == SourceBuffer)) {
    return 0;
  }

  ASSERT (DestinationBuffer != NULL);
  ASSERT (SourceBuffer != NULL);
  ASSERT ((Length - 1) <= (MAX_ADDRESS - (UINTN)DestinationBuffer));
  ASSERT ((Length - 1) <= (MAX_ADDRESS - (UINTN)SourceBuffer));

  DestinationBytes = DestinationBuffer;
  SourceBytes      = SourceBuffer;
  while ((--Length != 0) &&
         (*DestinationBytes == *SourceBytes))
  {
    DestinationBytes++;
    SourceBytes++;
  }

  return *DestinationBytes - *SourceBytes;
}

/**
  Fills a target buffer with a byte value, and returns the target buffer.

  This implementation is based on InternalMemSetMem from
  MU_BASECORE/MdePkg/Library/BaseMemoryLib/SetMem.c

  @param  Buffer    A pointer to the target buffer to fill.
  @param  Length    The number of bytes in Buffer to fill.
  @param  Value     The value with which to fill Length bytes of Buffer.

  @return Buffer.

**/
VOID *
EFIAPI
SetMem (
  OUT VOID  *Buffer,
  IN UINTN  Length,
  IN UINT8  Value
  )
{
  volatile UINT8   *Pointer8;
  volatile UINT32  *Pointer32;
  volatile UINT64  *Pointer64;
  UINT32           Value32;
  UINT64           Value64;

  if ((((UINTN)Buffer & 0x7) == 0) && (Length >= 8)) {
    // Generate the 64bit value
    Value32 = (Value << 24) | (Value << 16) | (Value << 8) | Value;
    Value64 = (((UINT64)Value32) << 32) | Value32;

    Pointer64 = (UINT64 *)Buffer;
    while (Length >= 8) {
      *(Pointer64++) = Value64;
      Length        -= 8;
    }

    // Finish with bytes if needed
    Pointer8 = (UINT8 *)Pointer64;
    while (Length-- > 0) {
      *(Pointer8++) = Value;
    }
  } else if ((((UINTN)Buffer & 0x3) == 0) && (Length >= 4)) {
    // Generate the 32bit value
    Value32 = (Value << 24) | (Value << 16) | (Value << 8) | Value;

    Pointer32 = (UINT32 *)Buffer;
    while (Length >= 4) {
      *(Pointer32++) = Value32;
      Length        -= 4;
    }

    // Finish with bytes if needed
    Pointer8 = (UINT8 *)Pointer32;
    while (Length-- > 0) {
      *(Pointer8++) = Value;
    }
  } else {
    Pointer8 = (UINT8 *)Buffer;
    while (Length-- > 0) {
      *(Pointer8++) = Value;
    }
  }

  return Buffer;
}

/**
  Scans a target buffer for an 8-bit value, and returns a pointer to the matching 8-bit value
  in the target buffer.

  This implementation is based on InternalMemScanMem8 from
  MU_BASECORE/MdePkg/Library/BaseMemoryLib/ScanMem8Wrapper.c

  @param  Buffer    A pointer to the target buffer to scan.
  @param  Length    The number of bytes in Buffer to scan.
  @param  Value     The value to search for in the target buffer.

  @return A pointer to the first occurrence of Value in Buffer.
  @retval NULL      If Length is 0 or Value was not found in Buffer.

**/
VOID *
EFIAPI
ScanMem8 (
  IN CONST VOID  *Buffer,
  IN UINTN       Length,
  IN UINT8       Value
  )
{
  CONST UINT8  *Pointer;

  Pointer = (CONST UINT8 *)Buffer;
  do {
    if (*Pointer == Value) {
      return (VOID *)Pointer;
    }

    ++Pointer;
  } while (--Length != 0);

  return NULL;
}

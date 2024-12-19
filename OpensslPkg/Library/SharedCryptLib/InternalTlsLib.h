/** @file
  Internal include file for TlsLib.

Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __INTERNAL_TLS_LIB_H__
#define __INTERNAL_TLS_LIB_H__

#undef _WIN32
#undef _WIN64
#include <CrtLibSupport.h>
#include <Protocol/Tls.h>
#include <IndustryStandard/Tls1.h>
#include <Library/SafeIntLib.h>
#include <Library/PcdLib.h>
#include <openssl/obj_mac.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include "Rand/CryptRand.h"

#pragma message("SharedCryptLib: using Shared/InternalTlsLib.h")

typedef struct {
  //
  // Main SSL Connection which is created by a server or a client
  // per established connection.
  //
  SSL    *Ssl;
  //
  // Memory BIO for the TLS/SSL Reading operations.
  //
  BIO    *InBio;
  //
  // Memory BIO for the TLS/SSL Writing operations.
  //
  BIO    *OutBio;
} TLS_CONNECTION;

#endif

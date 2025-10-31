
## @file
#  OneCrypto.inc.dsc includes the OneCrypto binary component
#
#  Copyright (c) Microsoft Corporation
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

# TODO rename SHARED_SHARED_CRYPTO_PATH to ONE_CRYPTO_PATH

[Components.X64]
  # Common - Precompiled - Shared Component across phases 
  $(SHARED_SHARED_CRYPTO_PATH)/bin/shared/OneCryptoMmBin.inf

  # Drivers that will be built from source to provide platform services
  $(SHARED_SHARED_CRYPTO_PATH)/bin/shared/OneCryptoDxeLoader.inf

  $(SHARED_SHARED_CRYPTO_PATH)/bin/shared/OneCryptoLoaderMm.inf

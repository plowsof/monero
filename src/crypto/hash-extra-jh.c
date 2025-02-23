// Copyright (c) The Monero Project
// 
// This source code is licensed under the BSD-3 license found in the
// LICENSE file in the root directory of this source tree.
// 
// 
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "jh.h"
#include "hash-ops.h"

void hash_extra_jh(const void *data, size_t length, char *hash) {
  int r = jh_hash(HASH_SIZE * 8, data, 8 * length, (uint8_t*)hash);
  assert(SUCCESS == r);
}

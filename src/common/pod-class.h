// Copyright (c) The Monero Project
// 
// This source code is licensed under the BSD-3 license found in the
// LICENSE file in the root directory of this source tree.
// 
// 
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#pragma once

// FIXME: Why is this ifdef needed?  Hopefully making it struct won't break things.

/*
#if defined(_MSC_VER)
#define POD_CLASS struct
#else
#define POD_CLASS class
#endif
*/

#define POD_CLASS struct

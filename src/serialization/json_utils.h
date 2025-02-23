// Copyright (c) The Monero Project
// 
// This source code is licensed under the BSD-3 license found in the
// LICENSE file in the root directory of this source tree.
// 
// 
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#pragma once

#include <sstream>
#include "json_archive.h"

namespace serialization {

template<class T>
std::string dump_json(T &v)
{
  std::stringstream ostr;
  json_archive<true> oar(ostr);
  assert(serialization::serialize(oar, v));
  return ostr.str();
};

} // namespace serialization

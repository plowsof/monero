# Copyright (c) The Monero Project
# 
# This source code is licensed under the BSD-3 license found in the
# LICENSE file in the root directory of this source tree.
#

set(transfers_sources
  transfers.cpp)

set(transfers_headers)

monero_add_minimal_executable(transfers
  ${transfers_sources}
  ${transfers_headers})
target_link_libraries(transfers
  PRIVATE
    useragent
    rpc
    cryptonote_core
    cncrypto
    common
    epee
    ${GTEST_LIBRARIES})

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test_transfers")
add_custom_target(test_transfers
  COMMAND           transfers
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test_transfers")

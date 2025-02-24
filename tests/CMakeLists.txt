# Copyright (c) The Monero Project
# 
# This source code is licensed under the BSD-3 license found in the
# LICENSE file in the root directory of this source tree.
# 
# 
# Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

set(MONERO_WALLET_CRYPTO_BENCH "auto" CACHE STRING "Select wallet crypto libraries for benchmarking")

# The docs say this only affects grouping in IDEs
set(folder "tests")
set(TEST_DATA_DIR "${CMAKE_CURRENT_LIST_DIR}/data")

if (WIN32 AND STATIC)
  add_definitions(-DSTATICLIB)
  # miniupnp changed their static define
  add_definitions(-DMINIUPNP_STATICLIB)
endif ()

find_package(GTest)

if (GTest_FOUND)
  include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})
else ()
  message(STATUS "GTest not found on the system: will use GTest bundled with this source")

  include(ExternalProject)
  ExternalProject_Add(googletest
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gtest
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/gtest
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    INSTALL_COMMAND ""
  )
  add_library(gtest      UNKNOWN IMPORTED)
  add_library(gtest_main UNKNOWN IMPORTED)
  set_target_properties(gtest PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/gtest/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}"
  )
  set_target_properties(gtest_main PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/gtest/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
  )
  add_dependencies(gtest      googletest)
  add_dependencies(gtest_main googletest)

  # Emulate the FindGTest module's variable.
  set(GTEST_LIBRARIES gtest gtest_main)
  set(GTEST_BOTH_LIBRARIES gtest gtest_main)
  include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/gtest/include")
endif (GTest_FOUND)

message(STATUS "Copying test data directory...")
file(COPY data DESTINATION .) # Copy data directory from source root to build root

if (CMAKE_BUILD_TYPE STREQUAL "fuzz" OR OSSFUZZ)
  add_subdirectory(fuzz)
else ()
  add_subdirectory(core_tests)
  add_subdirectory(fuzz)
  add_subdirectory(crypto)
  add_subdirectory(functional_tests)
  add_subdirectory(performance_tests)
  add_subdirectory(core_proxy)
  add_subdirectory(unit_tests)
  add_subdirectory(difficulty)
  add_subdirectory(block_weight)
  add_subdirectory(hash)
  add_subdirectory(net_load_tests)
endif()

if (BUILD_GUI_DEPS)
  add_subdirectory(libwallet_api_tests)
endif()

if (TREZOR_DEBUG)
  add_subdirectory(trezor)
endif()

# add_subdirectory(daemon_tests)

set(hash_targets_sources
  hash-target.cpp)

set(hash_targets_headers)

monero_add_minimal_executable(hash-target-tests
  ${hash_targets_sources}
  ${hash_targets_headers})
target_link_libraries(hash-target-tests
  PRIVATE
    cryptonote_core)
set_property(TARGET hash-target-tests
  PROPERTY
    FOLDER "${folder}")

add_test(
  NAME    hash-target
  COMMAND hash-target-tests)

#
# Configure wallet crypto benchmark
#
if (${MONERO_WALLET_CRYPTO_BENCH} STREQUAL "auto")
  set(MONERO_WALLET_CRYPTO_BENCH "cn")
  monero_crypto_autodetect(AVAILABLE BEST)
  if (DEFINED AVAILABLE)
    list(APPEND MONERO_WALLET_CRYPTO_BENCH ${AVAILABLE})
  endif ()
  message("Wallet crypto bench is using ${MONERO_WALLET_CRYPTO_BENCH}")
endif ()

list(REMOVE_DUPLICATES MONERO_WALLET_CRYPTO_BENCH)
list(REMOVE_ITEM MONERO_WALLET_CRYPTO_BENCH "cn") # always used for comparison
set(MONERO_WALLET_CRYPTO_BENCH_NAMES "(cn)")
foreach(BENCH IN LISTS MONERO_WALLET_CRYPTO_BENCH)
  monero_crypto_valid(${BENCH} VALID)
  if (NOT VALID)
    message(FATAL_ERROR "Invalid MONERO_WALLET_CRYPTO_BENCH option ${BENCH}")
  endif ()

  monero_crypto_get_target(${BENCH} BENCH_LIBRARY)
  list(APPEND BENCH_OBJECTS $<TARGET_OBJECTS:${BENCH_LIBRARY}>)

  monero_crypto_get_namespace(${BENCH} BENCH_NAMESPACE)
  set(MONERO_WALLET_CRYPTO_BENCH_NAMES "${MONERO_WALLET_CRYPTO_BENCH_NAMES}(${BENCH_NAMESPACE})")
endforeach ()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/benchmark.h.in" "${MONERO_GENERATED_HEADERS_DIR}/tests/benchmark.h")
monero_add_minimal_executable(monero-wallet-crypto-bench benchmark.cpp ${BENCH_OBJECTS})
target_link_libraries(monero-wallet-crypto-bench cncrypto)

add_test(NAME wallet-crypto-bench COMMAND monero-wallet-crypto-bench)


set(enabled_tests
    core_tests
    difficulty
    block_weight
    hash
    performance_tests
    core_proxy
    fuzz
    unit_tests)

add_custom_target(tests DEPENDS enabled_tests)
set_property(TARGET tests PROPERTY FOLDER "${folder}")

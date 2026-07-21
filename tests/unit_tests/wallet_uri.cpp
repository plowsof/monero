// Copyright (c) 2014-2026, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "gtest/gtest.h"

#include "wallet/wallet2.h"

namespace
{
  constexpr const char *DOCS_ADDRESS =
    "467iotZU5tvG26k2xdZWkJ7gwATFVhfbuV3yDoWx5jHoPwxEi4f5BuJQwkP6GpCb1sZvUVB7nbSkgEuW8NKrh9KKRRga5qz";

  constexpr const char *DOCS_SPEND_KEY =
    "029c559cd7669f14e91fd835144916009f8697ab5ac5c7f7c06e1ff869c17b0b";

  constexpr const char *DOCS_VIEW_KEY =
    "afaf646edbff3d3bcee8efd3383ffe5d20c947040f74e1110b70ca0fbb0ef90d";
}

TEST(wallet_uri, parse_docs_example_spend_view_keys_ok)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};
  ASSERT_TRUE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?spend_key=" + DOCS_SPEND_KEY + "&view_key=" + DOCS_VIEW_KEY + "&height=12345676",
    cryptonote::MAINNET, uri, error)) << error;

  ASSERT_EQ(uri.address, DOCS_ADDRESS);
  ASSERT_TRUE(uri.seed.empty());
  ASSERT_EQ(std::string(uri.spend_key.data(), uri.spend_key.size()), DOCS_SPEND_KEY);
  ASSERT_EQ(std::string(uri.view_key.data(), uri.view_key.size()), DOCS_VIEW_KEY);
  ASSERT_TRUE(bool(uri.height));
  ASSERT_EQ(*uri.height, 12345676u);
}

TEST(wallet_uri, parse_docs_example_seed_height_ok)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};
  ASSERT_TRUE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS +
      "?seed=python%20runway%20gossip%20lymph%20hills%20karate%20ruined%20innocent%20ought%20dual%20shipped%20shipped%20sushi%20pyramid%20guys%20entrance%20obedient%20natural%20kiwi%20wobbly%20vixen%20wipeout%20template%20typist%20innocent&height=12345676",
    cryptonote::MAINNET, uri, error)) << error;

  ASSERT_EQ(uri.address, DOCS_ADDRESS);
  ASSERT_EQ(std::string(uri.seed.data(), uri.seed.size()),
    "python runway gossip lymph hills karate ruined innocent ought dual shipped shipped sushi pyramid guys entrance obedient natural kiwi wobbly vixen wipeout template typist innocent");
  ASSERT_TRUE(bool(uri.height));
  ASSERT_EQ(*uri.height, 12345676u);
}

TEST(wallet_uri, parse_single_txid_ok)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};
  ASSERT_TRUE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?seed=alpha&txid=" + std::string(64, 'a'),
    cryptonote::MAINNET, uri, error)) << error;

  ASSERT_EQ(uri.txids.size(), 1u);
  ASSERT_EQ(epee::string_tools::pod_to_hex(uri.txids[0]), std::string(64, 'a'));
}

TEST(wallet_uri, parse_multiple_txid_ok)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};
  ASSERT_TRUE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?seed=alpha&txid=" + std::string(64, 'a') + ";" + std::string(64, 'b'),
    cryptonote::MAINNET, uri, error)) << error;

  ASSERT_EQ(uri.txids.size(), 2u);
  ASSERT_EQ(epee::string_tools::pod_to_hex(uri.txids[0]), std::string(64, 'a'));
  ASSERT_EQ(epee::string_tools::pod_to_hex(uri.txids[1]), std::string(64, 'b'));
}

TEST(wallet_uri, rejects_invalid_txid)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};
  ASSERT_FALSE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?seed=alpha&txid=not_a_txid",
    cryptonote::MAINNET, uri, error));
  ASSERT_FALSE(error.empty());
}

TEST(wallet_uri, rejects_height_and_txid_together)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};
  ASSERT_FALSE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?seed=alpha&height=42&txid=" + std::string(64, 'a'),
    cryptonote::MAINNET, uri, error));
  ASSERT_FALSE(error.empty());
}

TEST(wallet_uri, rejects_invalid_address)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};
  ASSERT_FALSE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:not_an_address?seed=alpha"),
    cryptonote::MAINNET, uri, error));
  ASSERT_FALSE(error.empty());
}

TEST(wallet_uri, rejects_seed_and_view_key_together)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};
  ASSERT_FALSE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?seed=alpha&view_key=" + DOCS_VIEW_KEY,
    cryptonote::MAINNET, uri, error));
  ASSERT_FALSE(error.empty());
}

TEST(wallet_uri, rejects_seed_and_spend_key_together)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};
  ASSERT_FALSE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?seed=alpha&spend_key=" + DOCS_SPEND_KEY,
    cryptonote::MAINNET, uri, error));
  ASSERT_FALSE(error.empty());
}

TEST(wallet_uri, rejects_spend_key_without_view_key)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};
  ASSERT_FALSE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?spend_key=" + DOCS_SPEND_KEY,
    cryptonote::MAINNET, uri, error));
  ASSERT_FALSE(error.empty());
}

TEST(wallet_uri, parse_view_key_only_ok)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};
  ASSERT_TRUE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?view_key=" + DOCS_VIEW_KEY,
    cryptonote::MAINNET, uri, error)) << error;

  ASSERT_TRUE(uri.spend_key.empty());
  ASSERT_EQ(std::string(uri.view_key.data(), uri.view_key.size()), DOCS_VIEW_KEY);
}

TEST(wallet_uri, rejects_missing_seed_and_view_key)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};
  ASSERT_FALSE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?height=42",
    cryptonote::MAINNET, uri, error));
  ASSERT_FALSE(error.empty());
}

TEST(wallet_uri, make_roundtrip_seed_height)
{
  std::string error;
  tools::wallet2::wallet_uri in{};
  in.address = DOCS_ADDRESS;
  in.seed = epee::wipeable_string("alpha beta", 10);
  in.height = 42;

  const std::string uri = tools::wallet2::make_wallet_uri(in, cryptonote::MAINNET, error);
  ASSERT_TRUE(error.empty()) << error;
  ASSERT_FALSE(uri.empty());

  tools::wallet2::wallet_uri out{};
  ASSERT_TRUE(tools::wallet2::parse_wallet_uri(uri, cryptonote::MAINNET, out, error)) << error;
  ASSERT_EQ(out.address, DOCS_ADDRESS);
  ASSERT_EQ(std::string(out.seed.data(), out.seed.size()), "alpha beta");
  ASSERT_TRUE(bool(out.height));
  ASSERT_EQ(*out.height, 42u);
}

TEST(wallet_uri, make_roundtrip_seed_txid)
{
  std::string error;
  tools::wallet2::wallet_uri in{};
  in.address = DOCS_ADDRESS;
  in.seed = epee::wipeable_string("alpha beta", 10);
  in.txids.push_back(crypto::hash{});

  const std::string uri = tools::wallet2::make_wallet_uri(in, cryptonote::MAINNET, error);
  ASSERT_TRUE(error.empty()) << error;
  ASSERT_FALSE(uri.empty());

  tools::wallet2::wallet_uri out{};
  ASSERT_TRUE(tools::wallet2::parse_wallet_uri(uri, cryptonote::MAINNET, out, error)) << error;
  ASSERT_EQ(out.txids.size(), 1u);
  ASSERT_EQ(out.txids[0], crypto::hash{});
}

TEST(wallet_uri, make_rejects_height_and_txid_together)
{
  std::string error;
  tools::wallet2::wallet_uri in{};
  in.address = DOCS_ADDRESS;
  in.seed = epee::wipeable_string("alpha beta", 10);
  in.height = 42;
  in.txids.push_back(crypto::hash{});

  const std::string uri = tools::wallet2::make_wallet_uri(in, cryptonote::MAINNET, error);
  ASSERT_TRUE(uri.empty());
  ASSERT_FALSE(error.empty());
}

TEST(wallet_uri, make_rejects_spend_key_without_view_key)
{
  std::string error;
  tools::wallet2::wallet_uri in{};
  in.address = DOCS_ADDRESS;
  in.spend_key = epee::wipeable_string(DOCS_SPEND_KEY);

  const std::string uri = tools::wallet2::make_wallet_uri(in, cryptonote::MAINNET, error);
  ASSERT_TRUE(uri.empty());
  ASSERT_FALSE(error.empty());
}

TEST(wallet_uri, make_rejects_invalid_spend_key)
{
  std::string error;
  tools::wallet2::wallet_uri in{};
  in.address = DOCS_ADDRESS;
  in.spend_key = epee::wipeable_string("not_hex");
  in.view_key = epee::wipeable_string(DOCS_VIEW_KEY);

  const std::string uri = tools::wallet2::make_wallet_uri(in, cryptonote::MAINNET, error);
  ASSERT_TRUE(uri.empty());
  ASSERT_FALSE(error.empty());
}

TEST(wallet_uri, make_rejects_invalid_view_key)
{
  std::string error;
  tools::wallet2::wallet_uri in{};
  in.address = DOCS_ADDRESS;
  in.view_key = epee::wipeable_string("not_hex");

  const std::string uri = tools::wallet2::make_wallet_uri(in, cryptonote::MAINNET, error);
  ASSERT_TRUE(uri.empty());
  ASSERT_FALSE(error.empty());
}

TEST(wallet_uri, make_roundtrip_view_key_only)
{
  std::string error;
  tools::wallet2::wallet_uri in{};
  in.address = DOCS_ADDRESS;
  in.view_key = epee::wipeable_string(DOCS_VIEW_KEY);

  const std::string uri = tools::wallet2::make_wallet_uri(in, cryptonote::MAINNET, error);
  ASSERT_TRUE(error.empty()) << error;
  ASSERT_FALSE(uri.empty());

  tools::wallet2::wallet_uri out{};
  ASSERT_TRUE(tools::wallet2::parse_wallet_uri(uri, cryptonote::MAINNET, out, error)) << error;
  ASSERT_TRUE(out.spend_key.empty());
  ASSERT_EQ(std::string(out.view_key.data(), out.view_key.size()), DOCS_VIEW_KEY);
}

TEST(wallet_uri, make_rejects_seed_with_equals_sign)
{
  std::string error;
  tools::wallet2::wallet_uri in{};
  in.address = DOCS_ADDRESS;
  in.seed = epee::wipeable_string("alpha=beta");

  const std::string uri = tools::wallet2::make_wallet_uri(in, cryptonote::MAINNET, error);
  ASSERT_TRUE(uri.empty());
  ASSERT_FALSE(error.empty());
}

TEST(wallet_uri, parse_does_not_partially_fill_result_on_failure)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};

  // second txid is invalid; the first would otherwise have been pushed into uri.txids
  ASSERT_FALSE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?seed=alpha&txid=" + std::string(64, 'a') + ";zz",
    cryptonote::MAINNET, uri, error));
  ASSERT_FALSE(error.empty());

  ASSERT_TRUE(uri.address.empty());
  ASSERT_TRUE(uri.seed.empty());
  ASSERT_TRUE(uri.txids.empty());
}

TEST(wallet_uri, parse_does_not_leak_previous_result_on_failure)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};

  // populate uri with a successful parse first
  ASSERT_TRUE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?seed=alpha",
    cryptonote::MAINNET, uri, error)) << error;
  ASSERT_FALSE(uri.seed.empty());

  // a subsequent failed parse must not leave the previous seed sitting in uri
  ASSERT_FALSE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:not_an_address?seed=beta"),
    cryptonote::MAINNET, uri, error));
  ASSERT_FALSE(error.empty());
  ASSERT_TRUE(uri.seed.empty());
  ASSERT_TRUE(uri.address.empty());
}

TEST(wallet_uri, error_does_not_echo_secrets)
{
  std::string error;
  tools::wallet2::wallet_uri uri{};

  const std::string seed = "some secret seed words";
  ASSERT_FALSE(tools::wallet2::parse_wallet_uri(
    std::string("not-a-scheme:") + DOCS_ADDRESS + "?seed=" + seed,
    cryptonote::MAINNET, uri, error));
  ASSERT_EQ(error.find(seed), std::string::npos);

  ASSERT_FALSE(tools::wallet2::parse_wallet_uri(
    std::string("monero-wallet:") + DOCS_ADDRESS + "?view_key=deadbeef",
    cryptonote::MAINNET, uri, error));
  ASSERT_EQ(error.find("deadbeef"), std::string::npos);
}

#ifndef SRC_BLOCKS_BLOCKS_H_
#define SRC_BLOCKS_BLOCKS_H_

#include <vector>

#include "cryptonote_config.h"
#include "span.h"

namespace blocks
{
    const epee::span<const unsigned char> GetCheckpointsData(cryptonote::network_type network);
    const epee::span<const uint64_t> GetRCTCoinbaseOutputDist(cryptonote::network_type network);

    std::string compress_one_span_format(const std::vector<uint64_t>& data);
    std::vector<uint64_t> decompress_one_span_format(const std::string& compressed);
}

#endif /* SRC_BLOCKS_BLOCKS_H_ */

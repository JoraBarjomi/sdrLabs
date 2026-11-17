#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

constexpr int TX_AMPLITUDE = 1000 << 4;

void map_bpsk(const std::vector<uint8_t>& bits, int16_t* buffer,
              size_t num_symbs);

void map_qpsk(const std::vector<uint8_t>& bits, int16_t* buffer,
              size_t num_symbs);
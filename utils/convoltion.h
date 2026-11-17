#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>

void upsample(const int16_t* in, int16_t* out, size_t num_symbols, size_t sps);

void convolve(const int16_t* in, size_t n, const int16_t* h, size_t h_len,
              int16_t* out);
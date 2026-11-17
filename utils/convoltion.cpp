
#include "convoltion.h"

void upsample(const int16_t* in, int16_t* out, size_t num_symbols, size_t sps) {
  std::fill(out, out + 2 * num_symbols * sps, 0);
  for (size_t s = 0; s < num_symbols; s++) {
    out[2 * s * sps] = in[2 * s];
    out[2 * s * sps + 1] = in[2 * s + 1];
  }
}

void convolve(const int16_t* in, size_t n, const int16_t* h, size_t h_len,
              int16_t* out) {
  for (size_t i = 0; i < n; i++) {
    int32_t acc_i = 0, acc_q = 0;
    for (size_t k = 0; k < h_len && k <= i; k++) {
      acc_i += in[2 * (i - k)] * h[k];
      acc_q += in[2 * (i - k) + 1] * h[k];
    }
    out[2 * i] = static_cast<int16_t>(acc_i);
    out[2 * i + 1] = static_cast<int16_t>(acc_q);
  }
}
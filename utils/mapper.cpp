#include "mapper.h"

void map_bpsk(const std::vector<uint8_t>& bits, int16_t* buffer,
              size_t num_symbols) {
  for (size_t sym = 0; sym < num_symbols; sym++) {
    uint8_t bit = bits[sym];

    buffer[2 * sym] = bit ? -TX_AMPLITUDE : TX_AMPLITUDE;
    buffer[2 * sym + 1] = 0;
  }
}

void map_qpsk(const std::vector<uint8_t>& bits, int16_t* buffer,
              size_t num_symbols) {
  for (size_t sym = 0; sym < num_symbols; sym++) {
    uint8_t b1 = bits[2 * sym], b2 = bits[2 * sym + 1];

    buffer[2 * sym] = b1 ? -TX_AMPLITUDE : TX_AMPLITUDE;
    buffer[2 * sym + 1] = b2 ? -TX_AMPLITUDE : TX_AMPLITUDE;
  }
}
#include "bitsGen.h"

void genBits(std::vector<uint8_t>& bits) {
  std::string s = "Hello, world!";

  for (char c : s) {
    std::bitset<8> bitsChar(static_cast<unsigned char>(c));

    for (int i = 7; i >= 0; i--) {
      bits.push_back(bitsChar[i]);
    }
  }
}
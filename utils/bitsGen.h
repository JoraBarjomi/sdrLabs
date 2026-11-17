#pragma once
#include <bitset>
#include <cstdint>
#include <string>
#include <vector>

/*
Передаю эти биты "Hello, world!"
|    H     |    e     |     l    |    l     |     o    |    ,     |  пробел  |    w     |    o     |    r     |    l     |    d     |    !     |
| 01001000 | 01100101 | 01101100 | 01101100 | 01101111 | 00101100 | 00101100 | 01110111 | 01101111 | 01110010 | 01101100 | 01100100 | 00100001 |
*/

void genBits(std::vector<uint8_t>& bits);

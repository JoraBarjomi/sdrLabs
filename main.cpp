#include <complex.h>
#include <stdint.h>  //int32_t uint16_t
#include <stdio.h>   //printf
#include <stdlib.h>  //free

#include <SoapySDR/Device.hpp>   // Инициализация устройства
#include <SoapySDR/Formats.hpp>  // Типы данных, используемых для записи сэмплов
#include <cstring>               // strcmp
#include <fstream>               // open close file stream
#include <iostream>              // cout
#include <string>

#include "sdr/sdr.hpp"

constexpr size_t iteration_count = 300;

constexpr long timeoutUs = 400000;

int main(int argc, char* argv[]) {
  char *sdrIp = NULL, *sdrMode = NULL;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-ip") == 0) {
      sdrIp = argv[++i];
    } else if (strcmp(argv[i], "-mode") == 0) {
      sdrMode = argv[++i];
    }
  }
  if (!sdrIp || !sdrMode) {
    std::cerr << "ERROR: Missing arguments!\nUsing '-ip <IP> -mode <MODE>'\n";
    return 2;
  }
  SDRMode mode;
  if (strcmp(sdrMode, "tx") == 0) {
    std::cout << "Init SDR with ip: " << sdrIp << " in " << sdrMode
              << " mode\n";
    mode = SDRMode::Tx;
  } else if (strcmp(sdrMode, "rx") == 0) {
    std::cout << "Init SDR with ip: " << sdrIp << " in " << sdrMode
              << " mode\n";
    mode = SDRMode::Rx;
  } else {
    std::cerr << "ERROR: Unknown SDR mode!\n";
    return 3;
  }

  auto mySdr = SDR(sdrIp, mode);

  int flags = 0;
  long long timeNs = 0;

  if (mode == SDRMode::Tx) {
    SoapySDR::Stream* txStream =
        mySdr.device()->setupStream(SOAPY_SDR_TX, SOAPY_SDR_CS16, {0});
    mySdr.device()->activateStream(txStream);
    size_t mtu = mySdr.device()->getStreamMTU(txStream);
    std::vector<int16_t> txBuffer(2 * mtu);
    void* buffs[] = {txBuffer.data()};

    for (size_t i = 0; i < 2 * mtu; i += 2) {
      txBuffer[i] = 777 << 4;
      txBuffer[i + 1] = 666 << 4;
    }

    for (size_t j = 0; j < iteration_count; j++) {
      int samples = mySdr.device()->writeStream(txStream, buffs, mtu, flags,
                                                timeNs, timeoutUs);
      std::cout << "Samples sent: " << samples << "\n";
    }

    mySdr.device()->deactivateStream(txStream);
    mySdr.device()->closeStream(txStream);

  } else if (mode == SDRMode::Rx) {
    // открыли файл для записи в него данных
    std::ofstream file;
    file.open("txdata.bin", std::ios::binary);

    SoapySDR::Stream* rxStream =
        mySdr.device()->setupStream(SOAPY_SDR_RX, SOAPY_SDR_CS16, {0});
    mySdr.device()->activateStream(rxStream);

    size_t mtu = mySdr.device()->getStreamMTU(rxStream);
    std::vector<int16_t> rxBuffer(2 * mtu);
    void* buffs[] = {rxBuffer.data()};

    for (size_t buffers_read = 0; buffers_read < iteration_count;
         buffers_read++) {
      int samples = mySdr.device()->readStream(rxStream, buffs, mtu, flags,
                                               timeNs, timeoutUs);
      std::cout << "Samples readed: " << samples << "\n";
      if (file.is_open() && samples > 0) {
        file.write(reinterpret_cast<const char*>(rxBuffer.data()),
                   2 * samples * sizeof(int16_t));
      }
    }
    mySdr.device()->deactivateStream(rxStream);
    mySdr.device()->closeStream(rxStream);
    file.close();
  }

  return 0;
}
#include <SoapySDR/Device.h>   // Инициализация устройства
#include <SoapySDR/Formats.h>  // Типы данных, используемых для записи сэмплов
#include <complex.h>
#include <stdint.h>
#include <stdio.h>   //printf
#include <stdlib.h>  //free

#include <cstring>
#include <iostream>
#include <string>

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
  int mode;
  if (strcmp(sdrMode, "tx") == 0) {
    std::cout << "Init SDR with ip: " << sdrIp << " in " << sdrMode
              << " mode\n";
    mode = 1;
  } else if (strcmp(sdrMode, "rx") == 0) {
    std::cout << "Init SDR with ip: " << sdrIp << " in " << sdrMode
              << " mode\n";
    mode = 0;
  } else {
    std::cerr << "ERROR: Unknown SDR mode!\n";
    return 3;
  }

  SoapySDRKwargs args = {};
  SoapySDRKwargs_set(&args, "driver",
                     "plutosdr");  // Говорим какой Тип устройства
  if (1) {
    SoapySDRKwargs_set(&args, "uri", "usb:");  // Способ обмена сэмплами (USB)
  } else {
    if (sdrIp != NULL) {
      std::string ip = std::string("ip:") + sdrIp;
      SoapySDRKwargs_set(&args, "uri", ip.c_str());  // Или по IP-адресу
    }
  }
  SoapySDRKwargs_set(&args, "direct", "1");  //
  SoapySDRKwargs_set(&args, "timestamp_every",
                     "1920");  // Размер буфера + временные метки
  SoapySDRKwargs_set(&args, "loopback", "0");  // Используем антенны или нет
  SoapySDRDevice* sdr = SoapySDRDevice_make(&args);  // Инициализация
  SoapySDRKwargs_clear(&args);

  if (sdr == NULL) {
    printf("SoapySDRDevice_make fail: %s\n", SoapySDRDevice_lastError());
    return EXIT_FAILURE;
  }

  // Настраиваем устройства RX\TX
  if (SoapySDRDevice_setSampleRate(sdr, SOAPY_SDR_RX, 0, 2e6) != 0) {
    printf("setSampleRate rx fail: %s\n", SoapySDRDevice_lastError());
  }
  if (SoapySDRDevice_setFrequency(sdr, SOAPY_SDR_RX, 0, 400e6, NULL) != 0) {
    printf("setFrequency rx fail: %s\n", SoapySDRDevice_lastError());
  }
  if (SoapySDRDevice_setSampleRate(sdr, SOAPY_SDR_TX, 0, 2e6) != 0) {
    printf("setSampleRate tx fail: %s\n", SoapySDRDevice_lastError());
  }
  if (SoapySDRDevice_setFrequency(sdr, SOAPY_SDR_TX, 0, 400e6, NULL) != 0) {
    printf("setFrequency tx fail: %s\n", SoapySDRDevice_lastError());
  }

  printf("SoapySDRDevice_getFrequency tx: %lf\n",
         SoapySDRDevice_getFrequency(sdr, SOAPY_SDR_TX, 0));

  // Инициализация количества каналов RX\\TX (в AdalmPluto он один, нулевой)
  size_t channels[] = {0};  // Настройки усилителей на RX\\TX
  // Чувствительность приемника
  if (SoapySDRDevice_setGain(sdr, SOAPY_SDR_RX, channels[0], 60.0) != 0) {
    std::cout << "setGain rx failed[" << SoapySDRDevice_lastError() << "]\n";
  }
  // Усиление передатчика
  if (SoapySDRDevice_setGain(sdr, SOAPY_SDR_TX, channels[0],
                             mode == 1 ? 75.0 : 0.0) != 0) {
    std::cout << "setGain tx failed[" << SoapySDRDevice_lastError() << "]\n";
  }

  size_t channel_count = sizeof(channels) / sizeof(channels[0]);
  // Формирование потоков для передачи и приема сэмплов
  SoapySDRStream* rxStream = SoapySDRDevice_setupStream(
      sdr, SOAPY_SDR_RX, SOAPY_SDR_CS16, channels, channel_count, NULL);
  SoapySDRStream* txStream = SoapySDRDevice_setupStream(
      sdr, SOAPY_SDR_TX, SOAPY_SDR_CS16, channels, channel_count, NULL);

  // Получение MTU (Maximum Transmission Unit), в нашем случае - размер буферов.
  size_t rx_mtu = SoapySDRDevice_getStreamMTU(sdr, rxStream);
  size_t tx_mtu = SoapySDRDevice_getStreamMTU(sdr, txStream);

  // Выделяем память под буферы RX и TX
  int16_t tx_buff[2 * tx_mtu];
  int16_t rx_buffer[2 * rx_mtu];

  int sign = 1;
  for (int i = 0; i < 2 * tx_mtu; i += 2) {
    sign *= -1;
    tx_buff[i] = 1000;
    tx_buff[i + 1] = 1000;
  }

  // prepare fixed bytes in transmit buffer
  // we transmit a pattern of FFFF FFFF [TS_0]00 [TS_1]00 [TS_2]00 [TS_3]00
  // [TS_4]00 [TS_5]00 [TS_6]00 [TS_7]00 FFFF FFFF that is a flag (FFFF FFFF)
  // followed by the 64 bit timestamp, split into 8 bytes and packed into the
  // lsb of each of the DAC words. DAC samples are left aligned 12-bits, so each
  // byte is left shifted into place
  for (size_t i = 0; i < 2; i++) {
    tx_buff[0 + i] = 0xffff;
    // 8 x timestamp words
    tx_buff[10 + i] = 0xffff;
  }

  SoapySDRDevice_activateStream(sdr, rxStream, 0, 0, 0);  // start streaming
  SoapySDRDevice_activateStream(sdr, txStream, 0, 0, 0);  // start streaming

  printf("Start test...\n");

  const long timeoutUs = 400000;
  long long last_time = 0;
  size_t errors = 0;
  size_t iteration_count = 10000;

  // При первом запуске отчищаем rx_buff от возможного мусора.
  for (size_t buffers_read = 0; buffers_read < 128; /* in loop */) {
    void* buffs[] = {rx_buffer};  // void *buffs[] = {rx_buff[0][0],
                                  // rx_buff[0][1]}; //array of buffers
    int flags;                    // flags set by receive operation
    long long timeNs;             // timestamp for receive buffer
    // Read samples
    int sr = SoapySDRDevice_readStream(sdr, rxStream, buffs, rx_mtu, &flags,
                                       &timeNs, timeoutUs);  // 100ms timeout
    if (sr < 0) {
      // Skip read on error (likely timeout)
      continue;
    }
    // Increment number of buffers read
    buffers_read++;
  }

  FILE* file = NULL;
  if (mode == 0) {
    file = fopen("rxdata.pcm", "wb");
    if (!file) std::cerr << "Cannot open rxdata.pcm\n";
  }

  // Начинается работа с получением и отправкой сэмплов
  for (size_t buffers_read = 0; buffers_read < iteration_count;) {
    void* rx_buffs[] = {rx_buffer};
    int flags;         // flags set by receive operation
    long long timeNs;  // timestamp for receive buffer

    // считали буффер RX, записали его в rx_buffer
    int sr = SoapySDRDevice_readStream(sdr, rxStream, rx_buffs, rx_mtu, &flags,
                                       &timeNs, timeoutUs);

    if (sr < 0) {
      std::cout << "RX error: " << sr << "\n";
      if (++errors > 100) break;
      continue;
    }

    if (mode == 0) {  // rx: печатаем и пишем в файл
      printf(
          "Buffer: %zu - Samples: %i, Flags: %i, Time: %lli, TimeDiff: %lli\n",
          buffers_read, sr, flags, timeNs, timeNs - last_time);
      if (file) fwrite(rx_buffer, sizeof(int16_t), 2 * sr, file);
    } else if (buffers_read % 2 == 0) {                // tx: шлём пакет
      long long tx_time = timeNs + (4 * 1000 * 1000);  // на 4 мс в будущее
      for (size_t i = 0; i < 8; i++) {
        uint8_t tx_time_byte = (tx_time >> (i * 8)) & 0xff;
        tx_buff[2 + i] = static_cast<int16_t>(tx_time_byte << 4);
      }
      const void* tx_buffs[] = {tx_buff};
      int txFlags = SOAPY_SDR_HAS_TIME | SOAPY_SDR_END_BURST;
      int st = SoapySDRDevice_writeStream(sdr, txStream, tx_buffs, tx_mtu,
                                          &txFlags, tx_time, timeoutUs);
      if ((size_t)st != tx_mtu) {
        printf("TX Failed: %i (%s)\n", st, SoapySDR_errToStr(st));
      } else {
        printf("Buffer: %zu - TX sent at %lli\n", buffers_read, tx_time);
      }
    }
    last_time = timeNs;
    buffers_read++;
  }

  if (file) fclose(file);

  // stop streaming
  SoapySDRDevice_deactivateStream(sdr, rxStream, 0, 0);
  SoapySDRDevice_deactivateStream(sdr, txStream, 0, 0);

  // shutdown the stream
  SoapySDRDevice_closeStream(sdr, rxStream);
  SoapySDRDevice_closeStream(sdr, txStream);

  // cleanup device handle
  SoapySDRDevice_unmake(sdr);

  return 0;
}
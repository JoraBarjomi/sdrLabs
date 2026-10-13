#pragma once
#include <SoapySDR/Device.hpp>  // Инициализация устройства
#include <iostream>
#include <string>

constexpr int sample_rate = 1e6;
constexpr int rx_freq = 1800e6;
constexpr int tx_freq = 1900e6;

enum SDRMode { Tx, Rx };

class SDR {
 public:
  SDR(std::string ip, SDRMode mode);
  ~SDR();

  SDR(const SDR&) = delete;
  SDR operator=(const SDR&) = delete;

  SoapySDR::Device* device() { return dev_; }

 private:
  std::string getSDRModeName(SDRMode mode);

  std::string ip_;
  SDRMode mode_;
  int carrier_freq_{};
  SoapySDR::Device* dev_{nullptr};
};
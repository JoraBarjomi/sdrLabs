#include "sdr.hpp"

std::string SDR::getSDRModeName(SDRMode mode) {
  switch (mode) {
    case SDRMode::Tx:
      return "Transmitter";
    case SDRMode::Rx:
      return "Receiver";
    default:
      return "Unknown";
  }
}

SDR::SDR(std::string ip, SDRMode mode) : ip_(ip), mode_(mode) {
  SoapySDR::Kwargs args;
  args["driver"] = "plutosdr";
  args["uri"] = "ip:" + ip;
  // args["direct"] = "0";
  // args["loopback"] = "0";
  // args["timestamp_every"] = "1920";
  dev_ = SoapySDR::Device::make(args);
  if (!dev_) {
    throw std::runtime_error("Failed to init SDR: " + ip + " in " +
                             getSDRModeName(mode) + " mode");
  }

  if (mode == SDRMode::Tx) {
    dev_->setSampleRate(SOAPY_SDR_TX, 0, sample_rate);
    dev_->setFrequency(SOAPY_SDR_TX, 0, tx_freq);
    dev_->setGain(SOAPY_SDR_TX, 0, -90.0);
  } else if (mode == SDRMode::Rx) {
    dev_->setSampleRate(SOAPY_SDR_RX, 0, sample_rate);
    dev_->setFrequency(SOAPY_SDR_RX, 0, rx_freq);
    dev_->setGain(SOAPY_SDR_RX, 0, 20.0);
  }
}

SDR::~SDR() {
  if (dev_) {
    SoapySDR::Device::unmake(dev_);
    dev_ = nullptr;
  }
}

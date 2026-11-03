# SDR Labs

Pluto SDR programms

### Requirements

- **Adalm PlutoSDR**
- **C++**
- **CMake**

### Build

```sh
git clone https://github.com/JoraBarjomi/sdrLabs
cd sdrLabs
mkdir build && cd build
cmake ..
make
```

### Run

Sdr running in two mods: rx, tx.

```bash
SoapySDRUtil --find // Find current sdr usb id

./sdr rx usb:1.5.5
or
./sdr tx usb:1.6.5
```

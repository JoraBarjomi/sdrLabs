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

```sh
./sdr -ip <IP> -mode <MODE>
```

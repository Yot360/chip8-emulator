# chip8-emulator
A chip-8 emulator made in C++ using SDL3 to render

## Building 
Clone the repo with submodules
```
git clone --recurse-submodules https://github.com/Yot360/chip8-emulator.git
```
```
mkdir build
cd build
```
Setup CMake
```
cmake ..
```
Build
```
cmake --build . -j$(nproc)
```

## Running
To run the emulator you need to pass a rom as argument in the command line:
```
./chip8-emu rom.ch8
```

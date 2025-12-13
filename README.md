# ADESS

*Adess* (artist directed engine sound synthetizer) is a CLI application for generating engine sounds. *Adess* doesn't generate the sound in real-time, rather using keyframes for animating the engine behaviour.

## Requirements:

- _CMake_ (`https://github.com/Kitware/CMake`)
- _CPU_ and _RAM_ (expensive)

## Installation:
### On Linux/MacOS:
1. clone the repository:
```bash
git clone https://github.com/LeonKubota/adess.git
```
2. compile using _CMake_:
```bash
cd build
cmake ..
make
```
I suggest adding *adess* to your `$PATH` for convenience.
### On Windows:
You have three installation options on Windows:
1. Use _WSL_
2. Do this:
   	1. Choose a Linux distribution and download it
	2. Create bootable installation media
   	3. Boot your Linux installation media
   	4. Install Linux
   	5. Install *adess* on Linux
4. Wait until I add Windows support (may not happen)

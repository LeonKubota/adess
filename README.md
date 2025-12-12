# ADESS

*Adess* (artist directed engine sound synthetizer) is a CLI application for generating engine sounds. *Adess* doesn't generate the sound in real-time, rather using keyframes for animating the engine behaviour.

## Requirements:

- _CMake_ (`https://github.com/Kitware/CMake`)

## Installation:
- On _Linux_ / _MacOS_
	- 1. clone the repository:
	```bash
	git clone https://github.com/LeonKubota/adess.git
	```
	- 2. compile:
	```bash
	cd build
	cmake ..
	make
	```
	I suggest adding *adess* to your `$PATH` for convenience.
- On _Windows_:
    - a) Use _WSL_
    - b) 
        - 1. Choose a _Linux_ distro and download it
        - 2. Create bootable installation media
        - 3. Boot your _Linux_ installation media
        - 4. Install _Linux_
        - 5. Install *adess* on _Linux_

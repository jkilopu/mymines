# mymines

Imitaion of classic game "Mines" using SDL2 library, with timer and flag.

## Gameplay

### Basic Gameplay

In the menu, you can determine the map width, height and the number of mines.

Once the game starts, you can use the mouse to click on blocks. The left click will open the block, the right click will set a flag on the block.

If you left-click on a digit, and the number of flags around it equals its value, then non-flagged blocks around it will be opened. 

On the right side of the game window, the time spent will be shown.

Sweep all the mines(open all the blocks which can be opened) to win!

### Game Mode

The mode of the game is set according to the number of args:
1 for local mode, 2 for server mode, 3 for client mode.

#### Local mode
There must be no arg.
``` bash
./mymines
```

#### LAN mode
* Server mode
``` bash
./mymines <port>
```
* Clinet mode
``` bash
./mymines <IP> <port>
```

## Requirements

* C/C++ compiler(gcc, MSVC, mingw-gcc)
* SDL2, SDL2_image, SDL2_net
* cmake
* make

## Installation

### Command Line
``` bash
git clone https://github.com/jkilopu/mymines.git 
cd ./mymines
git submodule update --init
mkdir build
cd build
cmake .. # You may need to specify a build system generator with "-G" option
make
cd ..
./build/bin/mymines
```

### Visual Studio 2019 with CMake

### Visual Studio Code with CMake

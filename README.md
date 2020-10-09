# mymine

Imitaion of classic game "Mines" using SDL2 library, with timer and flag.

## Gameplay

In the menu, you can determine the map width, height and the number of mines.

Once the game starts, you can use the mouse to click on blocks. The left click will open the block, the right click will set a flag on the block.

If you left-click on a digit, and the number of flags around it equals its value, then non-flagged blocks around it will be opened. 

On the right side of the game window, the time spent will be shown.

Sweep all the mines(open all the blocks which can be opened) to win!

## Requirements

* C/C++ compiler
* SDL2 library
* cmake
* make

## Installation

``` bash
git clone https://github.com/jkilopu/Linux-Programming.git
git submodule update --init
cd ./Linux-Programming/mymine
mkdir build
cd build
cmake .. # You may need to specify a build system generator with "-G" option
make
cd ..
./build/bin/mymine
```

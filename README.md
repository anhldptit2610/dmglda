# dmglda

A homebrew GB emulator.

I love playing games on 8/16-bit retro systems. Emulating them is even better, because  
I can understand how those are created.

## Dependencies

 * cmake version 3.27.7 or above
 * SDL2

## How to build

    $ git clone https://github.com/anhldptit2610/dmglda.git
    $ cd dmglda
    $ cmake -S . -B build
    $ cd build && make
    $ ./x86_64/dmglda /path/to/gb/rom/

## Images

![Super Mario Land](/Images/super_mario_land.jpg)
![Metroid II - Return of Samus](/Images/metroid.jpg)
![Tetris](/Images/tetris.jpg)
![Kirby's Dream Land](/Images/kirby.jpg)

## TODO

    * APU
    * Weird tall sprites bugs in The Legends of Zelda
    * Support more MBCs.
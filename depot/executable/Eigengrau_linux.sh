#!/bin/bash
cd bin/linux_x86_64

# set library path
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

# start game
if type gamemoderun &> /dev/null; then
    gamemoderun ./ProjectOne.elf "$@"
else
    ./ProjectOne.elf "$@"
fi
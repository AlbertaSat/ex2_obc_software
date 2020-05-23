## ex2 command handling demo
This repo will server as a generic example of how a board on the Ex-Alta 2 satellite will distribute commands, and invoke a service. This same design will be used across the system

## To run locally
* Clone [SatelliteSim](https://github.com/AlbertaSat/SatelliteSim/) into the same root directory as this repository (just to keep things simple) and follow the instructions in the SatelliteSim repo for building CSP

* For now, this still references the [upsat services](https://gitlab.com/librespacefoundation/upsat/upsat-ecss-services) library for some header files, so clone that too

* Compile this code to a static lib (.a) file, the command should be similar to the following,
```
gcc *.c Platform/demo/*.c Platform/demo/hal/*.c Services/*.c -c -I . -I Platform/demo -I Platform/hal -I Services/ -I ../upsat-ecss-services/services/ -I ../SatelliteSim/Source/include/ -I ../SatelliteSim/Project/ -I ../SatelliteSim/libcsp/include/ -I ../SatelliteSim/Source/portable/GCC/POSIX/ -I ../SatelliteSim/libcsp/build/include/ -m32 -lpthread -std=c99 -lrt && ar -rsc client_server.a *.o
```
* Link to this static library file in the SatelliteSim's Makefile (again, as described in the readme at that repo), and run
```make clean && make all```

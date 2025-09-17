# Nucci

### Overview
This repository include the source code of the counter on Geiger tube.  
MCU: STM32F407VG.

### Requirements
toolchain arm-none-eabi 14.2.1 or higher  
gcc / g++ 13.3.0 or higher  
cmake 3.14 or higher  
libjsoncpp-dev  

### Build project
>mkdir build  
>cd build  
>cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake -DCMAKE_BUILD_TYPE=Release  
>make -j8  

#### Start debug server
>./scripts/gdb-serv.sh openocd-jlink

#### Start debug client
>./scripts/gdb-client.sh

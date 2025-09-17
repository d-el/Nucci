#!/bin/bash

hexfile=${HEX:-$(dirname "$0")/../build/readed.bin}
echo "Hex file: ${hexfile}"

debugger=${DEBUGGER:-openocd-jlink}
echo "Debugger: ${debugger}"

$(dirname "$0")/gdb-serv.sh $debugger -c "reset halt" -c "flash read_bank 0 $hexfile  0x80000 147600" -c "reset" -c shutdown

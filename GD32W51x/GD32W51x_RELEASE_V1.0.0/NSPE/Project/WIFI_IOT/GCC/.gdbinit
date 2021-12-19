target remote :2331
set mem inaccessible-by-default off
monitor speed auto
monitor endian little
monitor reset
monitor flash breakpoints = 1
monitor flash download = 1
load
break Reset_Handler
break main
continue
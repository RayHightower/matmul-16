target remote :51011
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.34_39.elf
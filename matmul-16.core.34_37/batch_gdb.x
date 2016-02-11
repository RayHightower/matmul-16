target remote :51009
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.34_37.elf
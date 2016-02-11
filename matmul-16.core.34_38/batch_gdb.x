target remote :51010
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.34_38.elf
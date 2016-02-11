target remote :51008
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.34_36.elf
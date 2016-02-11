target remote :51012
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.35_36.elf
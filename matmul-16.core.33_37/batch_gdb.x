target remote :51005
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.33_37.elf
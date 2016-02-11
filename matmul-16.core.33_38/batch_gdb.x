target remote :51006
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.33_38.elf
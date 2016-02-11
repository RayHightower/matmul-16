target remote :51001
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.32_37.elf
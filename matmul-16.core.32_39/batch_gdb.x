target remote :51003
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.32_39.elf
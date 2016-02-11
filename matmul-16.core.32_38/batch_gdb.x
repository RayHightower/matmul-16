target remote :51002
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.32_38.elf
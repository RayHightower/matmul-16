target remote :51000
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.32_36.elf
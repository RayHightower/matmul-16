target remote :51015
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.35_39.elf
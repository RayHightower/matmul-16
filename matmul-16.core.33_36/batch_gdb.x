target remote :51004
load
c
monitor reset
q
# e-gdb -x batch_gdb.x matmul.core.33_36.elf
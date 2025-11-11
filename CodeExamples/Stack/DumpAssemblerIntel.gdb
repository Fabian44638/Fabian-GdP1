# Run like this:
# gdb -q -x DumpAssemblerIntel.gdb bin/swap-V1
set disassembly-flavor intel
disass /m main
quit

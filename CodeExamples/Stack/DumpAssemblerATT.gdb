# Run like this:
# gdb -q -x DumpAssemblerATT.gdb bin/swap-V1
set disassembly-flavor att
disass /m main
quit

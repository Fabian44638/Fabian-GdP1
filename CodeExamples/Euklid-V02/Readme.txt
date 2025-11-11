Diese Demo zeigt, wie man im Debugger GDB den Assembler-Code anzeigen kann,
der bei der Übersetzung eines C-Programms durch gcc entsteht.


Kurz im CLI:

gdb -q -ex "set disassembly-flavor intel"  -ex "disassemble main" bin/euklid




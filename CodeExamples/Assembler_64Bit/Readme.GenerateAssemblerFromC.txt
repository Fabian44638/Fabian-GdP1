#---------------------------------------------------
# Erzeuge AT&T-Assemblercode (UNIX Standard) aus einem C-Programm

# Mit folgendem Kommando kann man das C-Programm printValueWithPrintf_C.c
# in Assemblercode (Dialekt AT&T) übersetzen.

gcc -g -Wall -S -masm=att printValueWithPrintf_C.c

# Danach kann man die Assemblerdatei printValueWithPrintf_C.s betrachten.

# Erzeugung einer ausführbaren Datei
#
# Folgendes Kommando übersetzt den Assembler-Code
# in Maschinensprache (object code) und bindet
# diesen durch den Linker zur ausführbaren Datei

mkdir -p bin
gcc -g -Wall printValueWithPrintf_C.s -o bin/printValueWithPrintf_C

# Hint:
# Folgendes Kommando erzeugt aus printValueWithPrintf_C.c
# die Datei bin/printValueWithPrintf_C
# und (per Option -Wa,-adhln=printValueWithPrintf_C.lst) die Datei
# printValueWithPrintf_C.lst
# welche ausführliche Information darüber enthält, welcher
# C-Code die Ursache des erzeugten Maschinencodes war.

mkdir -p bin
gcc -Wa,-adhln=printValueWithPrintf_C.lst \
    -g -Wall printValueWithPrintf_C.c -o bin/printValueWithPrintf_C

# Cleanup

make clean

#-----------------------
# Hinweis:
# Die Assembler-Datei printValueWithPrintf_C_annotated.s
# ist eine manuell durch FABR anntotierte Version
# der Assembler-Datei printValueWithPrintf_C.s,
# die per Kommando
#   gcc -S -masm=att printValueWithPrintf_C.c
# erzeugt wurde.


# Build
#
# Important: compile with option -O0 (optimizer disabled)

make clean
make

#> output
mkdir bin
gcc -g -Wall -O0 initialized-vs-non-initialized-globals.c -o bin/initialized-vs-non-initialized-globals


#------------------------------------------
# Inspect the dump of the ELF object file
#------------------------------------------

# Dump the section .data (the section for initialized global variables)
# using utility objdump

$ objdump -s -j .data bin/initialized-vs-non-initialized-globals

bin/initialized-vs-non-initialized-globals:     Dateiformat elf64-x86-64

Inhalt von Abschnitt .data:
 201000 00000000 00000000 08102000 00000000  .......... .....
 201010 44332211                             D3".

==> 

The value of the initialized global variable gv_init is 
stored in the ELF binary file!

Beware of the litte endian format.i
The value 0x11223344 is stored as 44332211.

In the C-Code:

int gv_init  = 0x11223344;    // initialized global variable (see object dump)

In the dump:

 201010 44332211

# Dump information about the sections .data and .bss
# using utility readelf.

regensbu@tuxedo1:~/git_public_GdP1/CodeExamples/GlobalVars
$ readelf -SW bin/initialized-vs-non-initialized-globals  | egrep 'Nr|[.]data|[.]bss'
  [Nr] Name              Typ             Adresse          Off    Größe  ES Flg Lk Inf Al
  [23] .data             PROGBITS        0000000000201000 001000 000014 00  WA  0   0  8
  [24] .bss              NOBITS          0000000000201014 001014 00000c 00  WA  0   0  4

See
https://stackoverflow.com/questions/1597405/what-happens-to-a-declared-uninitialized-variable-in-c-does-it-have-a-value

START_QUOTE
The ELF standard (Remark FABR: https://www.sco.com/developers/devspecs/gabi41.pdf)
then guarantees that the section named .bss is completely filled with of zeros:

    .bss This section holds uninitialized data that contribute to the program’s
    memory image. By definition, the system initializes the data with zeros
    when the program begins to run. The section occu- pies no file space, as
    indicated by the section type, SHT_NOBITS.

Furthermore, the type SHT_NOBITS is efficient and occupies no space on the executable file:

    sh_size This member gives the section’s size in bytes. Unless the sec- tion
    type is SHT_NOBITS , the section occupies sh_size bytes in the file. A
    section of type SHT_NOBITS may have a non-zero size, but it occupies no
    space in the file.

Then it is up to the Linux kernel to zero out that memory region (Remarki FABR: .bss)
when loading the program into memory when it gets started.
END_QUOTE


#------------------------------------------
# Disassemble main in the debugger
#------------------------------------------

$ gdb  -q bin/initialized-vs-non-initialized-globals -ex "disassemble main" -ex "quit"
Reading symbols from bin/initialized-vs-non-initialized-globals...fertig.
Dump of assembler code for function main:
   0x000000000000064a <+0>:	push   %rbp
   0x000000000000064b <+1>:	mov    %rsp,%rbp
   0x000000000000064e <+4>:	mov    0x2009bc(%rip),%eax        # 0x201010 <gv_init>
   0x0000000000000654 <+10>:	mov    %eax,%esi
   0x0000000000000656 <+12>:	lea    0xb7(%rip),%rdi        # 0x714
   0x000000000000065d <+19>:	mov    $0x0,%eax
   0x0000000000000662 <+24>:	callq  0x520 <printf@plt>
   0x0000000000000667 <+29>:	mov    0x2009ab(%rip),%eax        # 0x201018 <gv_non_init>
   0x000000000000066d <+35>:	mov    %eax,%esi
   0x000000000000066f <+37>:	lea    0xab(%rip),%rdi        # 0x721
   0x0000000000000676 <+44>:	mov    $0x0,%eax
   0x000000000000067b <+49>:	callq  0x520 <printf@plt>
   0x0000000000000680 <+54>:	mov    $0x0,%eax
   0x0000000000000685 <+59>:	pop    %rbp
   0x0000000000000686 <+60>:	retq
End of assembler dump.

#------------------------------------------
# Disassemble main via objdump
#------------------------------------------

regensbu@tuxedo1:~/git_public_GdP1/CodeExamples/GlobalVars
$ objdump -d bin/initialized-vs-non-initialized-globals | less

...

000000000000064a <main>:
 64a:   55                      push   %rbp
 64b:   48 89 e5                mov    %rsp,%rbp
 64e:   8b 05 bc 09 20 00       mov    0x2009bc(%rip),%eax        # 201010 <gv_init>
 654:   89 c6                   mov    %eax,%esi
 656:   48 8d 3d b7 00 00 00    lea    0xb7(%rip),%rdi        # 714 <_IO_stdin_used+0x4>
 65d:   b8 00 00 00 00          mov    $0x0,%eax
 662:   e8 b9 fe ff ff          callq  520 <printf@plt>
 667:   8b 05 ab 09 20 00       mov    0x2009ab(%rip),%eax        # 201018 <__TMC_END__>
 66d:   89 c6                   mov    %eax,%esi
 66f:   48 8d 3d ab 00 00 00    lea    0xab(%rip),%rdi        # 721 <_IO_stdin_used+0x11>
 676:   b8 00 00 00 00          mov    $0x0,%eax
 67b:   e8 a0 fe ff ff          callq  520 <printf@plt>
 680:   b8 00 00 00 00          mov    $0x0,%eax
 685:   5d                      pop    %rbp
 686:   c3                      retq
 687:   66 0f 1f 84 00 00 00    nopw   0x0(%rax,%rax,1)
 68e:   00 00

Remark:
The value of the uninitialized global variable gv_non_init
is stored at the mark <__TMC_END__> where
TMC_END reads: transactional memory clone table end

See:
https://gcc.gnu.org/wiki/TransactionalMemory
https://stackoverflow.com/questions/17605794/what-are-linker-symbols-tmc-end-and-tmc-list-for



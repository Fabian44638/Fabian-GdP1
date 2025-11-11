# See the following URL for an explanation of the .* pragmas used
# https://stackoverflow.com/questions/17794533/
#          what-does-this-assembly-language-code-mean
#

        # Filename that contains the C source code 
	.file	"printValueWithPrintf_C.s"
	.text
        # Section read-only data; mapped to read-only page
	.section	.rodata
.LC0:   # .LCO is a label; acts as reference to value defined in next
        # line; here to string constant after .string,
        # which is the format specifier used in the printf statement.
	.string	"Wert=%d\n"

	# .text is the classical name for the code section
        .text
        # Create a visible function object for the linker (main)
	.globl	main
	.type	main, @function
main:   # Define entry point of function main
.LFB0:  # Local label for the "call frame information" .cfi_startproc,
        # which emits stack unwind info for exeption handlers and
        # debuggers.
	.cfi_startproc
        # Start of a standard function prologue in assembly code
	pushq	%rbp             # Save the frame pointer of the caller
	.cfi_def_cfa_offset 16   # More "call frame information"
	.cfi_offset 6, -16
	movq	%rsp, %rbp       # Set frame pointer of current frame


	.cfi_def_cfa_register 6  # More "call frame information"
        # End of function prologue

        # Assembly code for printf("Wert=%d\n", 25) 
	movl	$25, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
        # Assembly code for printf("Wert=%d\n", 16) 
	movl	$17, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT

        # Load return value of function into register eax
	movl	$0, %eax

        # Start epilogue of function
	popq	%rbp            # Restore frame pointer of caller
	.cfi_def_cfa 7, 8       # More "call frame information"
	ret                     # Return to caller
	.cfi_endproc            # More "call frame information"

.LFE0:  # More info about the executable file
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits

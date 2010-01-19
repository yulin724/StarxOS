
.text

.global load_gdt

load_gdt:
	movl 4(%esp), %eax
	lgdt (%eax)
	mov $0x10, %eax
	mov %eax, %ds
	mov %eax, %es
	mov %eax, %fs
	mov %eax, %gs
	mov %eax, %ss
	jmp $0x08, $load_gdt_ret
load_gdt_ret:
	ret

.global initialize_8259a
initialize_8259a:
	movb $0x11, %al
	outb %al, $0x20
	call iodelay
	
	outb %al, $0xa0
	call iodelay
	
	mov $0x20, %al
	outb %al, $0x21
	call iodelay
	
	mov $0x28, %al
	outb %al, $0xa1
	call iodelay
	
	mov $0x4, %al
	outb %al, $0x21
	call iodelay
	
	mov $0x2, %al
	outb %al, $0xa1
	call iodelay
	
	mov $0x1, %al
	outb %al, $0x21
	call iodelay
	
	out %al, $0xa1
	call iodelay
	
	mov $0b11111110, %al
#	mov $0b11111111, %al
	outb %al, $0x21
	call iodelay
	
	mov $0b11111111, %al
	outb %al, $0xa1
	call iodelay
	
	ret

iodelay:
	nop
	nop
	nop
	nop
	ret

do_IRQ:
	movl 4(%esp), %eax
	ret


/* macros for generating idt_entries */

.macro IRQ_STUB N TARGET
	.data; .long 1f; .text;
	1:
		// Trap 8, 10-14, 17
		// Supply an error number.  Else zero.
		.if (\N <> 8) && (\N < 10 || \N > 14) && ( \N <> 17 )
			pushl $0
		.endif
			pushl $\N
			jmp \TARGET
			.align 4
.endm

.macro IRQ_STUBS FIRST LAST TARGET
 irq=\FIRST
 .rept \LAST-\FIRST+1
	IRQ_STUB irq \TARGET
  irq=irq+1
 .endr
.endm

.data
.global idt_entries
idt_entries:
.text
	IRQ_STUBS 0 2 do_IRQ

	


/* The size of our stack (16KB). */
#define STACK_SIZE 0x4000

#define MULTIBOOT_HEADER_MAGIC 0x1BADB002

#define MULTIBOOT_HEADER_FLAGS 0x00000003

.text

.globl start, _start

start:
_start:
 jmp multiboot_entry

 /* Align 32 bits boundary. */
 .align  4

 /* Multiboot header. */
multiboot_header:
 /* magic */
 .long MULTIBOOT_HEADER_MAGIC
 /* flags */
 .long MULTIBOOT_HEADER_FLAGS
 /* checksum */
 .long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
 /* header_addr */
  .long multiboot_header
  .long code
  .long data
  .long bss
  .long end
 /* entry_addr */
  .long multiboot_entry
   
multiboot_entry:
 /* Initialize the stack pointer. */
 movl $(stack + 0x4000), %esp

 /* Reset EFLAGS. */
 pushl $0
 popf

 /* Push the pointer to the Multiboot information structure. */
 pushl %ebx
 /* Push the magic value. */
 pushl %eax

 /* Now enter the C main function... */
 cli
 call start_kernel

 /* Halt. */
 pushl $0
 pushl $halt_message
 pushl $0
 call sprintf

loop: hlt
  jmp loop

halt_message:
  .asciz  "Halted."

 /* Our stack area. */
 .comm stack, STACK_SIZE

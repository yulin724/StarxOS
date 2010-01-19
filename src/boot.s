;
; boot.s -- Kernel start location. Also defines multiboot header.
; Based on Bran's kernel development tutorial file start.asm
;

MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1<<1    ; Provide your kernel with memory info
MBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
; NOTE: We do not use MBOOT_AOUT_KLUDGE. It means that GRUB does not
; pass us a symbol table.
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

bits 32
global multibootheader
extern code
extern bss
extern end

multibootheader:
  dd  MBOOT_HEADER_MAGIC		; GRUB will search for this value on each
                                ; 4-byte boundary in your kernel file
  dd  MBOOT_HEADER_FLAGS		; How GRUB should load your file / settings
  dd  MBOOT_CHECKSUM			; To ensure that the above values are correct

  dd  multibootheader			; Location of this descriptor
  dd  code						; Start of kernel '.text' (code) section.
  dd  bss						; End of kernel '.data' section.
  dd  end						; End of kernel.
  dd  start						; Kernel entry point (initial EIP).

global start
extern start_kernel

start:
	mov esp, stack+0x4000
	push eax
	push ebx

	cli
	call start_kernel
	jmp $

section .bss
align 32
stack:
	resb 0x4000

.text

.global read_cr0
read_cr0:
    movl %cr0, %eax
    ret

.global write_cr0
write_cr0:
    pushl %ebp
    movl %esp, %ebp
    movl 8(%ebp), %eax
    movl %eax, %cr0
    popl %ebp
    ret

.global read_cr2
read_cr2:
    movl %cr2, %eax
    ret

.global read_cr3
read_cr3:
    movl %cr3, %eax
    ret

.global write_cr3
write_cr3:
    movl 4(%esp), %eax
    movl %eax, %cr3
    ret


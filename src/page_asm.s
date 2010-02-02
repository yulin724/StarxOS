.text

.global read_cr0
read_cr0:
    movl %cr0, %eax
    ret

.global write_cr0
write_cr0:
    pusha
    movl 4(%esp), %eax
    movl %eax, %cr0
    popa
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




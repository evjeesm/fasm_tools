format elf64
include 'mmap.inc'
include 'call.inc'
include 'io.inc'

section '.data' writable

ARCH_SET_FS = 0x1002
ARCH_SET_GS = 0x1003
ARCH_PRCTL = 158

section '.text' executable
    public _start

_start:
    SYSCALL MMAP, 0, 1024, DFLT_PROT, DFLT_FLAGS, 0, 0
    ; rax now contains the base address of the allocated TLS memory
    mov rsi, rax     ; Set base address for TLS
    mov rdi, ARCH_SET_FS
    mov rax, ARCH_PRCTL
    syscall

    mov qword [fs:0], 1000

.exit:
    mov rdi, rax
    EXIT rdi

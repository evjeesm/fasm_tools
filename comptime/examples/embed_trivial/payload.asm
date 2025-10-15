format elf64 executable 3
segment readable executable
entry _start

_start:
    mov rdi, 1
    lea rsi, [buf]
    mov rdx, buf.len
    mov rax, 1
    syscall

    xor rdi, rdi
    mov rax, 60
    syscall


segment readable
buf:
    db 'Hello, from Payload!'
    db 0xA
.len = $ - buf

segment readable writable



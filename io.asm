format elf64

include 'io.inc'
include 'errno.inc'
include 'call.inc'

section '.text' executable
    public write
    public full_write

;; Write without error checks.
write:
    SYSCALL WRITE, rdi, [rsi], rdx
    ret

full_write:
.again:
    mov rax, WRITE
    syscall
    test rax, rax
    js .error
    add rsi, rax
    sub rdx, rax
    test rdx, rdx ; success
    jz .exit
    jmp .again
.error:
    cmp rax, -EINTR
    je .again
    cmp rax, -EAGAIN
    je .again
    cmp rax, -EWOULDBLOCK
    je .again
.exit:
    ret

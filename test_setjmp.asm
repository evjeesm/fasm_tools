format elf64

include 'call.inc'
include 'io.inc'
include 'comptime.inc'
include 'errno.inc'
include 'inl_itoa.inc'
include 'signal.inc'
include 'setjmp.inc'

section '.text' executable
    public _start
    extrn setjmp
    extrn longjmp

_start:
    lea rsi, [mask]
    mov dword [rsi+0], 0x2222_2222
    mov dword [rsi+4], 0x2222_2222
    SYSCALL SIGPROCMASK, SIG_SETMASK, [mask], 0, SIGSET_SIZE
 
    call print_current_sigmask

    CCALL setjmp, [jmp_buf], 1
    push rax

    ;; int3 ;; sigprocmask should be restored
    call print_current_sigmask

    pop rax
    test eax, eax
    jnz .exit

    ;; change to process mask happens here
    lea rsi, [mask]
    mov dword [rsi+0], 0x1111_1111
    mov dword [rsi+4], 0x1111_1111
    SYSCALL SIGPROCMASK, SIG_BLOCK, [mask], 0, SIGSET_SIZE

    call print_current_sigmask
    CCALL longjmp, [jmp_buf], 1

.exit:
    mov rdi, rax
    SYSCALL EXIT, rdi

print_current_sigmask:
    push rbp
    mov rbp, rsp

    SYSCALL SIGPROCMASK, SIG_BLOCK, 0, [mask], SIGSET_SIZE
    ITOA [print_buf], [mask], 64, 16
    PRINT [print_buf], 16
    PRINT_INL 10

    leave
    ret

section '.data' writable
jmp_buf JMP_BUF
mask SIGSET_T
print_buf rb 16
          rb 1

FMT 'sizeof(JMP_BUF) = ' %D jmp_buf.size %NL

include 'comptime.inc'
include 'call.inc'
include 'signal.inc'
include 'ucontext.inc'

struc JMP_BUF
 {
common
.rbx rq 1
.rbp rq 1
.rsp rq 1
.r12 rq 1
.r13 rq 1
.r14 rq 1
.r15 rq 1
.rip rq 1
.sigmask_saved rw 1
.sigmask SIGSET_T
.size = $ - .rbx
 }

;; Mandatory list of API symbols
;; that will going to be exported
API setjmp
API longjmp

;; Implementation that will be embedded with IMPL macro invokation
macro __IMPLEMENTATION__
 {
CODE_SEGMENT
setjmp:
    mov [rdi + 0 ], rbx
    mov [rdi + 8 ], rbp
    mov [rdi + 16], rsp
    mov [rdi + 24], r12
    mov [rdi + 32], r13
    mov [rdi + 40], r14
    mov [rdi + 48], r15
    mov rax, [rsp]
    mov qword [rdi + 56], rax
    test rsi, rsi
    jz .exit
    mov word [rdi + 64], 1
    lea rax, [rdi + 66]
    SYSCALL SIGPROCMASK, SIG_BLOCK, 0, rax, SIGSET_SIZE
.exit:
    xor rax, rax
    ret

longjmp:
    push rsi
    mov rbx, [rdi + 0 ]
    mov rbp, [rdi + 8 ]
    mov rsp, [rdi + 16]
    mov r12, [rdi + 24]
    mov r13, [rdi + 32]
    mov r14, [rdi + 40]
    mov r15, [rdi + 48]
    mov r8 , [rdi + 56]
    mov si , [rdi + 64]
    test si, si
    jz .exit
    lea rsi, [rdi + 66] ;; rdi cant be used directly for arg1
                        ;; (used as arg0 in syscall)
    SYSCALL SIGPROCMASK, SIG_SETMASK, rsi, 0, SIGSET_SIZE
    pop rax ;; return value (rsi)
.exit:
    jmp r8
 };; __IMPLEMENTATION__

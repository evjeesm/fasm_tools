format elf64 executable 3
entry _start

include 'call.inc'
include 'mmap.inc'
include 'io.inc'

_start:
    SYSCALL MMAP, 0, 1024, DFLT_PROT, DFLT_FLAGS, 0, 0

    cmp rax, MMAP_FAILED
    je .mmap_failed

    push rax

    ; memory region with 'X'
    mov ecx, 1024
    mov rdi, rax
    mov al, 'X'
    rep stosb

    SYSCALL WRITE, 1, rax, 1024
    ;; mov rdi, 1
    ;; mov rsi, rax
    ;; mov rdx, 1024
    ;; mov rax, WRITE
    ;; syscall

    pop rdi
    SYSCALL MUNMAP, rdi, rsi
    ;; mov rax, MUNMAP
    ;; pop rdi         ; mapping address
    ;; ;mov rsi, 1024  ; size
    ;; syscall

.mmap_failed:
    SYSCALL EXIT, 0
    ;; mov rax, EXIT
    ;; xor rdi, rdi
    ;; syscall

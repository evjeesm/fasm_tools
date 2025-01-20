format elf64

section '.text' executable
    public _start

include 'mmap.inc'

; rdi  - preferable address
; edx  - protocol
; r9d  - offset
; r8   - fd (if you wanna map a file)
; r10d - flags
; rsi  - size

_start:
    mov rax, MMAP
    xor rdi, rdi                ; dest
    mov rsi, 1024               ; size
    mov edx,  PROT_R or PROT_W
    mov r10d, MAP_PRIVATE or MAP_ANON
    xor r8, r8                  ; fd
    xor r9, r9                  ; offset
    syscall

    cmp rax, -1
    je .mmap_failed

    push rax

    ; memory region with 'X'
    mov ecx, 1024
    mov rdi, rax
    mov al, 'X'
    rep stosb

    mov rsi, rax
    mov rdi, 1
    mov rdx, 1024
    mov rax, WRITE
    syscall

    mov rax, MUNMAP
    pop rdi         ; mapping address
    ;mov rsi, 1024  ; size
    syscall

    .mmap_failed:
    mov rax, EXIT
    xor rdi, rdi
    syscall

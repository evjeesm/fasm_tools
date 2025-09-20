READ = 0
WRITE = 1
OPEN = 2
CLOSE = 3
LSEEK = 7
PREAD = 17
PWRITE = 18
PIPE = 22
DUP2 = 33
NANOSLEEP = 35
FORK = 57
EXECVE = 59
EXIT = 60
TRUNCATE = 76
FTRUNCATE = 77
CHMOD = 90
FCHMOD = 91
GETRANDOM = 318

STDIN = 0
STDOUT = 1
STDERR = 2

;; Read from file descriptor.
;; (OBSOLETE use SYSCALL instead)
;;
macro READ fd*, addr*, size*
 {
    mov rax, READ
    mov rdi, fd
    lea rsi, addr
    mov rdx, size
    syscall
 }

;; Write to file descriptor, without error checks.
;; (OBSOLETE use SYSCALL instead)
;;
macro WRITE fd*, addr*, size*
 {
    mov rax, WRITE
    mov rdi, fd
    lea rsi, addr
    mov rdx, size
    syscall
 }

;; Write with error checks.
;; (OBSOLETE use 'write_all' function)
;;
macro WRITE_ALL fd*, addr*, size*
 {
  local .again, .error, .exit
    mov rdi, fd
    lea rsi, addr
    mov rdx, size
.again:
    mov rax, WRITE
    syscall
    test rax, rax
    js .error
    add rsi, rax
    sub rdx, rax
    test rdx, rdx ;; success
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
 }


macro PRINT addr*, size* { WRITE STDOUT, addr, size }

;; print inline message (message will be allocated inplace in the code)
macro PRINT_INL [bytes*]
 {
  common
    local .msg
    jmp @f
    .msg BUFFER bytes
    @@:
    PRINT [.msg], .msg#.size
 }

macro ERROR addr*, size* { WRITE STDERR, addr, size }

;; print inline message (message will be allocated inplace in the code)
macro ERROR_INL [bytes*]
 {
  common
    local .msg
    jmp @f
    .msg BUFFER bytes
    @@:
    ERROR [.msg], .msg#.size
 }

;; System exit. (OBSOLETE, use SYSCALL)
macro EXIT code*
 {
    mov rax, EXIT
    mov rdi, code
    syscall
 }

struc BUFFER [args*]
 {
 common
    . db args
    .size = $ - .
 }

macro NANOSLEEP timespec*
 {
   mov rax, NANOSLEEP  ; Syscall number for nanosleep
   lea rdi, timespec   ; Pointer to timespec structure
   xor rsi, rsi        ; NULL (no remaining 
   syscall
 }

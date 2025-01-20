; rdi  - preferable address
; edx  - protocol
; r9d  - offset
; r8   - fd (if you wanna map a file)
; r10d - flags
; rsi  - size

WRITE = 1
BRK   = 12
EXIT  = 60

; rax
MMAP = 9
MUNMAP = 11

; edx 
PROT_0 = 0x00
PROT_R = 0x01
PROT_W = 0x02
PROT_X = 0x04

; r10d
MAP_FIXED    = 0x0010
MAP_SHARED   = 0x0001
MAP_PRIVATE  = 0x0002
MAP_ANON     = 0x0020
MAP_STACK    = 0x4000

DFLT_FLAGS = MAP_ANON or MAP_PRIVATE
DFLT_PROT  = PROT_R or PROT_W

macro MMAP \
  length*, \
  prot:DFLT_PROT, \
  flags:DFLT_FLAGS, \
  addr:0, \
  offset:0, \
  fd:0
 {
    mov rax, MMAP
    mov rdi, addr
    mov rsi, length
    mov edx, prot
    mov r10d, flags
    mov r8, fd
    mov r9, offset
    syscall
 }

macro MUNMAP addr*, length*
 {
        mov rax, MUNMAP
  if ~ addr eq rdi
        mov rdi, addr
  end if
        mov rsi, length
        syscall
 }


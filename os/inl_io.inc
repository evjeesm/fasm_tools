include 'call.inc'

macro PRINT addr*, size* { SYSCALL WRITE, STDOUT, addr, size }

;; print inline message (message will be allocated inplace in the code)
macro PRINT_INL [bytes*]
 {
 common
  local .msg, .code
    jmp .code
.msg BUFFER bytes
.code:
    PRINT [.msg], .msg#.size
 }

macro ERROR addr*, size* { SYSCALL WRITE, STDERR, addr, size }

;; print inline message (message will be allocated inplace in the code)
macro ERROR_INL [bytes*]
 {
 common
  local .msg
    jmp .code
.msg BUFFER bytes
.code:
    ERROR [.msg], .msg#.size
 }

struc BUFFER [args*]
 {
 common
    . db args
    .size = $ - .
 }


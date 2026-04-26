include 'arch/x86_64/syscall.inc'
include 'arch/x86_64/call.inc'
include 'arch/x86_64/syscall_nr.inc'
include 'arch/x86_64/pointer.inc'
include '../descriptors.inc'

include 'inl_itoa.inc'
include 'inl_cstr.inc'
include 'vtext.inc'

;;
;; Print null-terminated string.
;; cstr - memory pointer with DEREF syntax
purge PRINT_CSTR
macro PRINT_CSTR cstr*
 {
  DEREF r10, cstr
  CSTR_LEN rdx, [r10]
  PRINT [r10], rdx
 }

;;
;; Print integer value for debug purposes.
purge PRINT_VALUE
macro PRINT_VALUE reg*, msg:''
 {
  local BUF_LEN
    BUF_LEN = 24 ;; Allocated on stack for formatting numbers
    mov r15, reg
  if msg eq ''
    PRINT_INL `reg, ": "
  else
    PRINT_INL msg
  end if
    sub rsp, BUF_LEN
    ITOA [rsp], r15, 64, 10, 0
    PRINT rsp, rdx
    PRINT_INL 0xA
    add rsp, BUF_LEN
}

;; Print inline message
;; (message will be allocated inplace in the code)
purge PRINT_INL
macro PRINT_INL [bytes*]
 {
 common
  local .msg, .code
  jmp .code
  .msg BUFFER bytes
  .code:
    PRINT [.msg], .msg#.size
 }

purge PRINT_STACK
macro PRINT_STACK [bytes*]
 {
 common
  VTEXT _bytes, bytes
  VTEXT_BAKE_STACK _bytes
  PRINT [rsp], _bytes#.len
  add rsp, _bytes#.len
 }

;; Print inline message
;; (message will be allocated inplace in the code)
purge ERROR_INL
macro ERROR_INL [bytes*]
 {
 common
  local .msg, .code
  jmp .code
  .msg BUFFER bytes
  .code:
    ERROR [.msg], .msg#.size
 }

purge PRINT
macro PRINT addr*, size* { SYSCALL WRITE, STDOUT, addr, size }
purge ERROR
macro ERROR addr*, size* { SYSCALL WRITE, STDERR, addr, size }

;; Stores bytes and its size (comptime)
restruc BUFFER
struc BUFFER [args*]
 {
 common
  . db args
  .size = $ - .
 }


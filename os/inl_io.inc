include 'call.inc'
include 'syscall.inc'
include 'pointer.inc'
include 'inl_itoa.inc'
include 'inl_cstr.inc'
include 'vtext.inc'

;;
;; Print null-terminated string.
;; cstr - memory pointer with DEREF syntax
macro PRINT_CSTR cstr*
 {
  DEREF r10, cstr
  CSTR_LEN rdx, [r10]
  PRINT [r10], rdx
 }

;;
;; Print integer value for debug purposes.
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
macro PRINT_INL [bytes*]
 {
 common
  local .msg, .code
    jmp .code
.msg BUFFER bytes
.code:
    PRINT [.msg], .msg#.size
 }


;; macro PRINT_STACK [bytes*]
;;  {
;;  common
;;     VTEXT _bytes, bytes
;;     sub rsp, _bytes#.len
;;     VTEXT_BAKE rsp, _bytes, _bytes#.len
;;     PRINT [rsp], _bytes#.len
;;     add rsp, _bytes#.len
;;  }

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
macro ERROR_INL [bytes*]
 {
 common
  local .msg
    jmp .code
.msg BUFFER bytes
.code:
    ERROR [.msg], .msg#.size
 }

macro PRINT addr*, size* { SYSCALL WRITE, STDOUT, addr, size }
macro ERROR addr*, size* { SYSCALL WRITE, STDERR, addr, size }

;; Stores bytes and its size (comptime)
struc BUFFER [args*]
 {
 common
    . db args
    .size = $ - .
 }


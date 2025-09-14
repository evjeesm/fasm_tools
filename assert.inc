include 'comptime.inc'

;; Enum for the sign
;; [TODO] use IOTA
_OP_EQ = 0
_OP_NE = 1
_OP_GT = 2
_OP_LT = 3
_OP_GE = 4
_OP_LE = 5

_OP_ZERO = 6
_OP_NOT_ZERO = 7
_OP_POSITIVE = 8
_OP_NEGATIVE = 9


ASSERT_ARG_LEN = 64
ASSERT_STR_LEN = 1024

ASSERT_COLOR_FG_RED equ "[31m"
ASSERT_COLOR_NONE equ "[0m"

;; macro TO_STR dest,src { ITOA dest,src,64,10,0,LOWER }

;;
;; Bake hardcoded text of the arg in 'assert_arg'
;; then copy into an 'assert_str'
;; 
macro ASSERT_STR_ARG arg
 {
 local ..text, n, d, dest
  virtual at 0
    ..text:: irps a, arg \{
      ;; capturing down whole expression
      ;; (negative literal case)
      db \`a
    \}
    ..text_len = $
  end virtual
  if ..text_len > ASSERT_ARG_LEN

      display \
        ASSERT_COLOR_FG_RED, \
        "ASSERT ERROR: Argument doesn't fit into a buffer", \
        ASSERT_COLOR_NONE

      err
  end if
  dest = assert_arg
  n = 0
  d = 4
  while n < ..text_len
    repeat (..text_len-n)/d
      local _STORE
      macro _STORE dest, src, size, offset
      \{
       local chunk
          load chunk size from src:offset
          mov size [dest+offset], chunk
      \}
      if d = 4
        _STORE dest, ..text, dword, n
      else if d = 2
        _STORE dest, ..text, word, n
      else if d = 1
        _STORE dest, ..text, byte, n
      end if
      n = n + d
    end repeat
    d = d / 2
  end while
  CCALL assert_str_build, [assert_arg], ..text_len
 }

;; OPS: EQ, NE, GT, LT, LE, GE
macro _ASSERT _test_, op, value, expected, msg
 {
 DEBUG
 local .ok
    pushfq
    push rax ;; [NOTE] push dont work on 32 and 8 bit regs
    push r10

    mov rax, value
    mov r10, expected

    _test_ rax, r10

    match =_OP_EQ, op \{ je  .ok \}
    match =_OP_NE, op \{ jne .ok \}
    match =_OP_GT, op \{ jg  .ok \}
    match =_OP_LT, op \{ jl  .ok \}
    match =_OP_GE, op \{ jge .ok \}
    match =_OP_LE, op \{ jle .ok \}
    CCALL assert_str_begin
    CCALL assert_str_build, [assert_red], assert_red_len
    CCALL assert_str_build, [assert_m], assert_m_len
    CCALL assert_str_build, [assert_clear], assert_clear_len

    ASSERT_STR_ARG value

    CCALL assert_str_op, op
    ASSERT_STR_ARG expected

    CCALL assert_str_build, [assert_comma], 2
    CCALL assert_str_build, [assert_obr], 1
    CCALL assert_str_parse, rax

    CCALL assert_str_op, op
    CCALL assert_str_parse, r10
    CCALL assert_str_build, [assert_cbr], 1
    CCALL assert_str_build, [assert_space], 1

    CCALL assert_str_build, [assert_red], assert_red_len
    ASSERT_STR_ARG msg
    CCALL assert_str_build, [assert_clear], assert_clear_len
    ERROR [assert_str], [assert_str_index]

    SYSCALL EXIT, -1
.ok:
    pop r10
    pop rax
    popfq
  END_DEBUG
 }

macro _ASSERT_TEST op, value, msg
 {
 DEBUG
 local .ok
    pushfq
    push rax ;; [NOTE] push dont work on 32 and 8 bit regs

    mov rax, value

    test rax, rax

    match =_OP_ZERO, op \{ jz  .ok \}
    match =_OP_NOT_ZERO, op \{ jnz .ok \}
    match =_OP_POSITIVE, op \{ jg  .ok \}
    match =_OP_NEGATIVE, op \{ js  .ok \}

    CCALL assert_str_begin
    CCALL assert_str_build, [assert_red], assert_red_len
    CCALL assert_str_build, [assert_m], assert_m_len
    CCALL assert_str_build, [assert_clear], assert_clear_len

    ASSERT_STR_ARG value

    CCALL assert_str_op, op

    CCALL assert_str_build, [assert_comma], 2
    CCALL assert_str_build, [assert_obr], 1
    CCALL assert_str_parse_signed, rax

    CCALL assert_str_op, op
    CCALL assert_str_build, [assert_cbr], 1
    CCALL assert_str_build, [assert_space], 1

    CCALL assert_str_build, [assert_red], assert_red_len
    ASSERT_STR_ARG msg
    CCALL assert_str_build, [assert_clear], assert_clear_len
    ERROR [assert_str], [assert_str_index]

    SYSCALL EXIT, -1
.ok:
    pop rax
    popfq
  END_DEBUG
 }

macro _GEN_ASSERTS [args*]
 {
 common
 irp arg, args \{
  macro ASSERT_\#arg ...& \\{
  common
     _ASSERT_CMP _OP_\\#arg, ...
 \\}
 \}
 }
;; Following source will be generated:
;; macro ASSERT_EQ [...]
;;  {
;;  common
;;     _ASSERT_CMP _OP_EQ, ...
;;  }
_GEN_ASSERTS EQ, NE, GT, LT, GE, LE

macro _GEN_UNARRY_ASSERTS [args*]
 {
 common
 irp arg, args \{
  macro ASSERT_\#arg ...& \\{
  common
     _ASSERT_TEST _OP_\\#arg, ...
 \\}
 \}
 }
;; Following source will be generated:
;; macro ASSERT_ZERO value, msg
;;  {
;;  common
;;    _ASSERT_TEST _OP_ZERO, value, msg
;;  }
_GEN_UNARRY_ASSERTS ZERO, NOT_ZERO, POSITIVE, NEGATIVE


macro ASSERT_POSITIVE value, msg
 {
 common
   _ASSERT_TEST _OP_POSITIVE, value, msg
 }

macro ASSERT_NEGATIVE value, msg
 {
 common
   _ASSERT_TEST _OP_NEGATIVE, value, msg
 }

postpone
 {
DEBUG
  segment readable executable

;; rdi - dest,
;; rsi - src,
;; rdx - dest offset,
;; rcx - src size,
;; r8 ,r9
assert_strcpy:
    ;; push rbp
    ;; mov rbp, rsp
    add rdi, rdx
    cld
    rep movsb

    ;; pop rbp
    ret

assert_str_begin:
    mov [assert_str_index], 0
    ret

;; rdi - src,
;; rsi - src_len,
;; rdx, rcx, r8, r9
assert_str_build:
    mov rcx, rsi
    mov rsi, rdi
    mov rdx, [assert_str_index]
    mov r8, rcx

    ;; CCALL assert_strcpy, [assert_str], rsi, rdx, rcx
    lea rdi, [assert_str]
    call assert_strcpy

    add [assert_str_index], r8
    ret

;;
;; rdi - src,
assert_str_parse:
    mov rsi, rdi
    lea rdi, [assert_str]
    mov rax, [assert_str_index]
    add rdi, rax

    ;; maybe replace in the future
    ;; or wrap as routine
    ITOA rdi, rsi, 64, 10, 0, LOWER

    add [assert_str_index], rdx
    mov rax, [assert_str_index]
    ret

;;
;; rdi - src,
assert_str_parse_signed:
    mov rsi, rdi
    lea rdi, [assert_str]
    mov rax, [assert_str_index]
    add rdi, rax

    ;; maybe replace in the future
    ;; or wrap as routine
    ITOA rdi, rsi, 64, 10, 1, LOWER

    add [assert_str_index], rdx
    mov rax, [assert_str_index]
    ret
;;
;;
;; rdi - enum of the sign
assert_str_op:
    jmp @f
.signs:
    dq assert_eq
    dq assert_ne
    dq assert_gt
    dq assert_lt
    dq assert_ge
    dq assert_le
    dq assert_zero
    dq assert_not_zero
    dq assert_positive
    dq assert_negative
.signs.sizes:
    db assert_eq_len
    db assert_ne_len
    db assert_gt_len
    db assert_lt_len
    db assert_ge_len
    db assert_le_len
    db assert_zero_len
    db assert_not_zero_len
    dq assert_positive_len
    dq assert_negative_len
@@:

    movzx rsi, byte [rdi + .signs.sizes]
    lea rdi, [.signs+(rdi*8)]
    mov rdi, [dword edi]

    CCALL assert_str_build, rdi, rsi
    ret

  segment readable
    assert_red db ASSERT_COLOR_FG_RED
    assert_red_len = $ - assert_red

    assert_clear db ASSERT_COLOR_NONE
    assert_clear_len = $ - assert_clear

    assert_m db "[ASSERT] "
    assert_m_len = $ - assert_m

    assert_zero:
    assert_eq db " == "
    assert_eq_len = $ - assert_eq
    db "0"
    assert_zero_len = $ - assert_zero

    assert_not_zero:
    assert_ne db " != "
    assert_ne_len = $ - assert_ne
    db "0"
    assert_not_zero_len = $ - assert_not_zero

    assert_positive:
    assert_gt db " > "
    assert_gt_len = $ - assert_gt
    db "0"
    assert_positive_len = $ - assert_positive

    assert_negative:
    assert_lt db " < "
    assert_lt_len = $ - assert_lt
    db "0"
    assert_negative_len = $ - assert_negative

    assert_ge db " >= "
    assert_ge_len = $ - assert_ge

    assert_le db " <= "
    assert_le_len = $ - assert_le



    assert_comma db ","
    assert_space db " "

    assert_obr db "("
    assert_cbr db ")"

  segment readable writable
    assert_arg rb ASSERT_ARG_LEN

    assert_str_index dq 0
    assert_str rb ASSERT_STR_LEN
END_DEBUG
 }


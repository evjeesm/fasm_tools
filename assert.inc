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

_OP_RANGES_EQ = 0
_OP_RANGES_NE = 1
_OP_RANGES_BYTE_EQ = 2
_OP_RANGES_BYTE_NE = 3


ASSERT_ARG_LEN = 64
ASSERT_STR_LEN = 1024

ASSERT_COLOR_FG_RED equ "[31m"
ASSERT_COLOR_NONE equ "[0m"

;;
;; Classical compile time assert with message.
;;
macro STATIC_ASSERT cond*, msg*
 {
  if ~(cond)
     display \
       ASSERT_COLOR_FG_RED, \
       "STATIC ASSERT: ", msg, \
       ASSERT_COLOR_NONE
     err
  end if
 }

;;
;; Puts source code location to an assert message.
;;
macro ASSERT_LOCATION file*, line*
 {
 local ..text, text_len
  virtual at 0
    ..text:: db file, "["
    text_len = $
  end virtual

  lea rdi, [assert_str]
  add rdi, [assert_str_index]

  _BAKE_TEXT rdi, ..text, text_len

  add [assert_str_index], text_len

  CCALL assert_str_parse, line
  ASSERT_STR_ARG "] "
 }

;;
;; Bake hardcoded text of the arg into the assert message.
;;
macro ASSERT_STR_ARG arg
 {
 local ..text, text_len
  virtual at 0
    ..text:: irps a, arg \{
      ;; capturing down whole expression
      ;; (negative literal case)
      db \`a
    \}
    text_len = $
  end virtual

  lea rdi, [assert_str]
  add rdi, [assert_str_index]

  _BAKE_TEXT rdi, ..text, text_len
  add [assert_str_index], text_len
 }

;;
;; Hardcode a text defined in a virtual block at 'vsrc' label.
;; Macro tries to store text using less mov instructions.
;;
macro _BAKE_TEXT dest*, vsrc*, text_len
 {
  local n, d
  n = 0
  d = 4
  while n < text_len
    repeat (text_len-n)/d
      local _STORE
      macro _STORE dest, src, size, offset
      \{
       local chunk
          load chunk size from src:offset
          mov size [dest+offset], chunk
      \}
      if d = 4
        _STORE dest, vsrc, dword, n
      else if d = 2
        _STORE dest, vsrc, word, n
      else if d = 1
        _STORE dest, vsrc, byte, n
      end if
      n = n + d
    end repeat
    d = d / 2
  end while
 }

;; OPS: EQ, NE, GT, LT, LE, GE
macro _ASSERT_CMP op, value, expected, msg
 {
 DEBUG
 local .ok
    pushfq
    push rax ;; [NOTE] push dont work on 32 and 8 bit regs
    push r10

    mov rax, value
    mov r10, expected

    cmp rax, r10

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
    ASSERT_LOCATION __FILE__, __LINE__

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
_GEN_UNARRY_ASSERTS ZERO, NOT_ZERO, POSITIVE, NEGATIVE

macro _GEN_RANGES_ASSERTS [args*]
 {
 common
 irp arg, args \{
  macro ASSERT_\#arg ...& \\{
  common
     _ASSERT_RANGES _OP_\\#arg, ...
 \\}
 \}
 }
_GEN_RANGES_ASSERTS RANGES_EQ, RANGES_NE, RANGES_BYTE_EQ, RANGES_BYTE_NE

macro _ASSERT_RANGES op, value, expected, size, msg
 {
 DEBUG
 local .ok
    pushfq
    push rdi ;; [NOTE] push dont work on 32 and 8 bit regs
    push rsi
    push rcx

    lea rdi, [expected]
    lea rsi, [value]
    mov rcx, size

    repeat 1
      match =_OP_RANGES_EQ, op \{
        repe cmpsb
        je .ok
      break
      \}
      match =_OP_RANGES_NE, op \{
        repe cmpsb
        jne .ok
      break
      \}
      match =_OP_RANGES_BYTE_NE, op \{
        repne cmpsb
        jne .ok
      break
      \}
      match =_OP_RANGES_BYTE_EQ, op \{
        repne cmpsb
        je .ok
      break
      \}
      STATIC_ASSERT 0, "OP not implemented!"
    end repeat

    push rcx ; store mismatch point

    CCALL assert_str_begin

    ; [ASSERT] 
    CCALL assert_str_build, [assert_red], assert_red_len
    CCALL assert_str_build, [assert_m], assert_m_len
    CCALL assert_str_build, [assert_clear], assert_clear_len

    ASSERT_LOCATION __FILE__, __LINE__
    ASSERT_STR_ARG "RANGES "
    CCALL assert_str_ranges_op, op
    ASSERT_STR_ARG '(actual:'#value#', expected:'#expected#')'
    CCALL assert_str_build, assert_nl, 1

    ;; Print matching part
    mov rcx, [rsp]
    sub rcx, size - 1
    neg rcx
    ;; CCALL assert_str_build, value, rcx
    CCALL assert_str_hex, value, rcx

    ;; Print mismatched char
    CCALL assert_str_build, assert_red, assert_red_len
    mov rdi, [rsp]
    sub rdi, size - 1
    neg rdi
    lea rdi, [value + rdi]
    push rdi
    inc qword [rsp]
    CCALL assert_str_hex, rdi, 1
    ;; CCALL assert_str_build, rdi, 1
    CCALL assert_str_build, [assert_clear], assert_clear_len

    ;; Print rest
    mov rcx, [rsp+8]
    pop rdi
    CCALL assert_str_hex, rdi, rcx
    ;; CCALL assert_str_build, rdi, rcx
    CCALL assert_str_build, assert_nl, 1

    ;; Print expected
    ;; CCALL assert_str_build, expected, size
    CCALL assert_str_hex, expected, size
    CCALL assert_str_build, assert_nl, 1

    ; [ASSERT] 
    CCALL assert_str_build, [assert_red], assert_red_len
    CCALL assert_str_build, [assert_m], assert_m_len
    ASSERT_STR_ARG msg
    CCALL assert_str_build, [assert_clear], assert_clear_len

    ERROR [assert_str], [assert_str_index]
    pop rcx
    SYSCALL EXIT, -1
.ok:
    pop rcx
    pop rsi
    pop rdi
    popfq
  END_DEBUG
 }

postpone
 {
DEBUG
  segment readable executable

;; Copy amount of src bytes to dest at specific offset
;; Args:
;; rdi - dest,
;; rsi - src,
;; rdx - dest offset,
;; rcx - src size
assert_strcpy:
    add rdi, rdx
    cld
    rep movsb
    ret

;; Resets assert message string
;; NOTE: (no need for that, cus assert will exit anyway)
assert_str_begin:
    mov [assert_str_index], 0
    ret

;; Copy string to assert message buffer advancing the index.
;; rdi - src,
;; rsi - src_len,
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

;; Parse contents of the register as unsigned integer
;; into the assert message buffer
;; rdi - src
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

;; Parse contents of the register as signed integer
;; into the assert message buffer
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

;; Parse contents of the register as hexadecimal
;; into the assert message buffer
;; rdi - src,
;; rsi - length,
assert_str_hex:
    mov rcx, rsi
    mov rsi, rdi
    test rcx, rcx
    jz .exit
.l:
    push rcx
    push rsi
    lea rdi, [assert_str]
    mov rax, [assert_str_index]
    add rdi, rax
    ITOA rdi, [rsi], 8, 16, 1, UPPER, ' '
    add [assert_str_index], rdx
    pop rsi
    pop rcx
    inc rsi
    dec rcx
    jnz .l

    mov rax, [assert_str_index]
.exit:
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
    db assert_positive_len
    db assert_negative_len
@@:

    movzx rsi, byte [rdi + .signs.sizes]
    lea rdi, [.signs+(rdi*8)]
    mov rdi, [dword edi]

    CCALL assert_str_build, rdi, rsi
    ret
;;
;;
;; rdi - enum of the sign
assert_str_ranges_op:
    jmp @f
.signs:
    dq assert_ranges_missmatch
    dq assert_ranges_match
.signs.sizes:
    db assert_ranges_missmatch_len
    db assert_ranges_match_len
@@:
    ;; rdi % 2
    push rax
    push rbx
    push rdx
    xor rdx, rdx
    mov rbx, 2
    mov rax, rdi
    div rbx
    mov rdi, rdx
    pop rdx
    pop rbx
    pop rax

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

    assert_nl db 10

    assert_ranges_missmatch db "miss"
    assert_ranges_match db "match "
    assert_ranges_missmatch_len = $ - assert_ranges_missmatch
    assert_ranges_match_len = $ - assert_ranges_match

  segment readable writable
    assert_str_index dq 0
    assert_str rb ASSERT_STR_LEN
END_DEBUG
 }


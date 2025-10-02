include 'comptime.inc'
include 'call.inc'
include 'inl_itoa.inc'
include 'inl_io.inc'

API assert_print_message
API assert_str_begin
API assert_str_flush
API assert_str_build
API assert_str_build_reg
API assert_str_op
API assert_str_ranges_op
API assert_str_format
API assert_str_format_signed
API assert_str_format_byte_hex
API assert_str_pad
API assert_str_range_actual
API assert_str_range_expected

;; Usage: (in routine)
;;   ASSERT_EQ value, expected, msg
;;  ... rest for other op's
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

;; Usage: (in routine)
;;   ASSERT_RANGE_ALL_EQ actual, expected, size, msg
;; or:
;;   ASSERT_RANGE_CHAR_ALL_EQ actual, char, size, msg
;;  ... rest for other op's
_OP_RANGE_ALL_EQ = 0
_OP_RANGE_ANY_NE = 1
_OP_RANGE_ANY_EQ = 2
_OP_RANGE_ALL_NE = 3

ASSERT_STR_LEN = 1024

ASSERT_COLOR_FG_RED equ "[31m"
ASSERT_COLOR_NONE equ "[0m"
ASSERT_NL equ 10

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

  ASSERT_BAKE_TEXT ..text, text_len
  CCALL assert_str_format, line
  ASSERT_STR_LITERAL "] "
 }

;;
;; Bake hardcoded text of the arg into the assert message.
;;
macro ASSERT_STR_ARG [arg*]
 {
 common
  local ..text, text_len
    virtual at 0
      ..text::
 forward
    irps a, arg \{
      ;; capturing down whole expression
      ;; (negative literal case)
        db \`a
    \}
 common
      text_len = $
    end virtual

  ASSERT_BAKE_TEXT ..text, text_len
 }

;;
;;
;;
macro ASSERT_STR_LITERAL [arg*]
 {
 common
  local ..text, text_len
    virtual at 0
      ..text::
 forward
        db arg
 common
      text_len = $
    end virtual

  ASSERT_BAKE_TEXT ..text, text_len
 }

macro ASSERT_BAKE_TEXT vsrc*, text_len*
 {
  local _STORE, offset, chunk, chunk_sz
  offset = 0
  chunk_sz = 8
  macro _STORE chunk \{
    if chunk_sz = 8
      load chunk qword from vsrc:offset
    else if chunk_sz = 4
      load chunk dword from vsrc:offset
    else if chunk_sz = 2
      load chunk word  from vsrc:offset
    else if chunk_sz = 1
      load chunk byte  from vsrc:offset
    end if
    CCALL assert_str_build_reg, chunk, chunk_sz
  \}
  while offset < text_len
    repeat (text_len-offset)/chunk_sz
      _STORE chunk
      offset = offset + chunk_sz
    end repeat
    chunk_sz = chunk_sz / 2
  end while
 }


macro _ASSERT_CMP op*, value*, expected*, msg*
 {
 DEBUG
 local .ok
    pushfq
    push rax
    push r10

    mov rax, value
    mov r10, expected
    cmp rax, r10
    repeat 1
      match =_OP_EQ, op \{
        je  .ok
      break
      \}
      match =_OP_NE, op \{
        jne  .ok
      break
      \}
      match =_OP_GT, op \{
        jg  .ok
      break
      \}
      match =_OP_LT, op \{
         jl  .ok
      break
      \}
      match =_OP_GE, op \{
        jge .ok
      break
      \}
      match =_OP_LE, op \{
        jle .ok
      break
      \}
      STATIC_ASSERT 0, `op#" is not implemented yet!"
    end repeat

    push r10
    push rax

    CCALL assert_print_message
    ASSERT_LOCATION __FILE__, __LINE__

    ASSERT_STR_ARG value

    CCALL assert_str_op, op
    ASSERT_STR_ARG expected

    ASSERT_STR_LITERAL ",("
    CCALL assert_str_format, *[rsp] ;; value

    CCALL assert_str_op, op
    CCALL assert_str_format, *[rsp+8] ;; expected
    ASSERT_STR_LITERAL ") "

    ASSERT_STR_LITERAL ASSERT_COLOR_FG_RED, msg, ASSERT_COLOR_NONE, ASSERT_NL

    CCALL assert_str_flush

    add rsp, 8*5 ;; clean stack
    SYSCALL EXIT, -1
.ok:
    pop r10
    pop rax
    popfq
 END_DEBUG
 }

macro _ASSERT_TEST op*, value*, msg*
 {
 DEBUG
 local .ok
    pushfq
    push rax

    mov rax, value

    test rax, rax
    repeat 1
      match =_OP_ZERO, op \{
        jz  .ok
      break
      \}
      match =_OP_NOT_ZERO, op \{
        jnz .ok
      break
      \}
      match =_OP_POSITIVE, op \{
        jg  .ok
      break
      \}
      match =_OP_NEGATIVE, op \{
        js  .ok
      break
      \}
      STATIC_ASSERT 0, `op#" is not implemented yet!"
    end repeat

    CCALL assert_print_message

    ASSERT_STR_ARG value

    CCALL assert_str_op, op

    ASSERT_STR_LITERAL ", ("
    CCALL assert_str_format_signed, rax
    CCALL assert_str_op, op
    ASSERT_STR_LITERAL ") ", ASSERT_COLOR_FG_RED, \
        msg, ASSERT_COLOR_NONE, ASSERT_NL

    CCALL assert_str_flush

    add rsp, 8*2 ;; clean stack
    SYSCALL EXIT, -1
.ok:
    pop rax
    popfq
 END_DEBUG
 }

 macro _ASSERT_RANGE op*, actual*, expected*, size*, msg*
 {
 DEBUG
 local .ok
    pushfq
    push rdi
    push rsi
    push rcx

    lea rdi, [expected]
    lea rsi, [actual]
    mov rcx, size

    repeat 1
      match =_OP_RANGE_ALL_EQ, op \{
        repe cmpsb
        je .ok
      break
      \}
      match =_OP_RANGE_ANY_NE, op \{
        repe cmpsb
        jne .ok
      break
      \}
      match =_OP_RANGE_ALL_NE, op \{
        repne cmpsb
        jne .ok
      break
      \}
      match =_OP_RANGE_ANY_EQ, op \{
        repne cmpsb
        je .ok
      break
      \}
      STATIC_ASSERT 0, `op#" is not implemented yet!"
    end repeat

    sub rcx, size - 1
    neg rcx
    push rcx ; store assert offset

    CCALL assert_print_message

    ASSERT_LOCATION __FILE__, __LINE__

    CCALL assert_str_ranges_op, op
    ASSERT_STR_ARG '(actual:', actual, ', expected:', expected ,')'
    ASSERT_STR_LITERAL ASSERT_NL

    CCALL assert_str_range_actual, [actual], *[rsp], size
    CCALL assert_str_range_expected, [expected], *[rsp], size

    ASSERT_STR_LITERAL ASSERT_COLOR_FG_RED, msg, ASSERT_COLOR_NONE, ASSERT_NL

    CCALL assert_str_flush

    pop rcx
    add rsp, 8*4 ;; clean stack
    SYSCALL EXIT, -1
.ok:
    pop rcx
    pop rsi
    pop rdi
    popfq
 END_DEBUG
 }

macro _ASSERT_RANGE_CHAR op*, actual*, char*, size*, msg*
 {
 DEBUG
 local .ok
    pushfq
    push rdi
    push rcx

    lea rdi, [actual]
    mov rax, char
    mov rcx, size

    repeat 1
      match =_OP_RANGE_ALL_EQ, op \{
        repe scasb
        je .ok
      break
      \}
      match =_OP_RANGE_ANY_NE, op \{
        repe scasb
        jne .ok
      break
      \}
      match =_OP_RANGE_ALL_NE, op \{
        repne scasb
        jne .ok
      break
      \}
      match =_OP_RANGE_ANY_EQ, op \{
        repne scasb
        je .ok
      break
      \}
      STATIC_ASSERT 0, `op#" is not implemented yet!"
    end repeat

    sub rcx, size - 1
    neg rcx
    push rcx ; store assert offset

    CCALL assert_print_message

    ASSERT_LOCATION __FILE__, __LINE__

    CCALL assert_str_ranges_op, op
    ASSERT_STR_ARG '(actual:', actual, ', char:', "'", char, "' "
    CCALL assert_str_format_byte_hex, char
    ASSERT_STR_LITERAL ')', ASSERT_NL

    CCALL assert_str_range_actual, [actual], *[rsp], size
    ASSERT_STR_LITERAL ASSERT_COLOR_FG_RED, msg, ASSERT_COLOR_NONE, ASSERT_NL

    CCALL assert_str_flush

    pop rcx
    add rsp, 8*3 ;; clean stack
    SYSCALL EXIT, -1
.ok:
    pop rcx
    pop rdi
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

macro _GEN_RANGE_ASSERTS [args*]
 {
 common
 irp arg, args \{
  macro ASSERT_RANGE_\#arg ...& \\{
  common
     _ASSERT_RANGE _OP_RANGE_\\#arg, ...
 \\}
 \}
 }
_GEN_RANGE_ASSERTS ALL_EQ, ANY_NE, ANY_EQ, ALL_NE

macro _GEN_RANGE_CHAR_ASSERTS [args*]
 {
 common
 irp arg, args \{
  macro ASSERT_RANGE_CHAR_\#arg ...& \\{
  common
     _ASSERT_RANGE_CHAR _OP_RANGE_\\#arg, ...
 \\}
 \}
 }
_GEN_RANGE_CHAR_ASSERTS ALL_EQ, ANY_NE, ANY_EQ, ALL_NE

macro __IMPLEMENTATION__
 {
DEBUG
CODE_SEGMENT

assert_print_message:
    ASSERT_STR_LITERAL \
        ASSERT_COLOR_FG_RED, \
        "[ASSERT] ", \
        ASSERT_COLOR_NONE
    ret

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

assert_str_flush:
    ERROR [assert_str], [assert_str_index]
    CCALL assert_str_begin
    ret


;; Copy string to assert message buffer advancing the index.
;; rdi - src,
;; rsi - src_len
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


;; Copy bytes from a register to assert message buffer
;; rdi - src bytes in a register
;; rsi - amount of bytes to copy [1..8]
assert_str_build_reg:
    push rbx
    lea rbx, [assert_str]
    add rbx, [assert_str_index]
    mov rcx, rsi
    mov r10, rsi
    cld
.copy:
    mov [rbx], dil
    shr rdi, 8
    inc rbx
    loop .copy

    add [assert_str_index], r10 ;; update index

    pop rbx
    ret

;; Format register as an unsigned integer onto 'assert_str'
;; rdi - src
assert_str_format:
    mov rsi, rdi
    lea rdi, [assert_str]
    add rdi, [assert_str_index]

    ;; maybe replace in the future
    ;; or wrap as routine
    ITOA rdi, rsi, 64, 10, 0, LOWER

    add [assert_str_index], rdx
    mov rax, [assert_str_index]
    ret

;; Format register as a signed integer onto 'assert_str'
;; rdi - src
assert_str_format_signed:
    mov rsi, rdi
    lea rdi, [assert_str]
    add rdi, [assert_str_index]

    ;; maybe replace in the future
    ;; or wrap as routine
    ITOA rdi, rsi, 64, 10, 1, LOWER

    add [assert_str_index], rdx
    mov rax, [assert_str_index]
    ret

;; Format byte array as a hexadecimal onto 'assert_str'
;; rdi - src,
;; rsi - length
assert_str_build_hex:
    mov rcx, rsi
    mov rsi, rdi
    test rcx, rcx
    jz .exit
.next:
    push rcx
    push rsi
    lea rdi, [assert_str]
    add rdi, [assert_str_index]
    ITOA rdi, [rsi], 8, 16, 1, UPPER, ' '
    add [assert_str_index], rdx
    pop rsi
    pop rcx
    inc rsi
    dec rcx
    jnz .next

    mov rax, [assert_str_index]
.exit:
    ret

;; Format byte in a hex format
assert_str_format_byte_hex:
    mov rsi, rdi
    lea rdi, [assert_str]
    add rdi, [assert_str_index]
    ITOA rdi, rsi, 8, 16, 1, UPPER, ' '
    add [assert_str_index], rdx
    ret

;; Appends padding to assert string
;; using specific character and length.
;; rdi - char
;; rsi - length
assert_str_pad:
    mov rax, rdi
    mov rcx, rsi
    lea rdi, [assert_str]
    add rdi, [assert_str_index]
    add [assert_str_index], rsi
    rep stosb
    ret

;; Print actual range
;; rdi - actual
;; rsi - assert point
;; rdx - size
assert_str_range_actual:
    push rbp
    mov rbp, rsp

    push rdx
    dec qword [rsp] ;; size - 1

    push rdi ;; range
    push rsi ;; assert_offset

    CCALL .print, [assert_str_build_hex] ;; Hex
    CCALL assert_str_pad, ' ', 4
    CCALL .print, [assert_str_build] ;; Verbatim
    ASSERT_STR_LITERAL ASSERT_NL

    add rsp, 8*3
    pop rbp
    ret

;; Subroutine that defines a template for printing actual range
;; rdi - print_func
.print:
    label .outer_scope   at rbp+8+8       ;; skip rbp, ret
    label .assert_offset at .outer_scope
    label .range_ptr     at .outer_scope+8
    label .size          at .outer_scope+16

    push rbp
    mov rbp, rsp

    push rbx ;; callee-preserved
    push rdi
    label .print_func at rbp-16

    ;; Print matching part
    mov rbx, [.print_func]
    CCALL rbx, *[.range_ptr], *[.assert_offset]

    ;; Print mismatched char
    ASSERT_STR_LITERAL ASSERT_COLOR_FG_RED
    mov rdi, [.assert_offset]
    add rdi, [.range_ptr]
    push rdi
    inc qword [rsp]
    CCALL rbx, [rdi], 1
    ASSERT_STR_LITERAL ASSERT_COLOR_NONE

    ;; Print rest
    mov rcx, [.assert_offset]
    sub rcx, [.size]
    neg rcx
    pop rdi
    CCALL rbx, [rdi], rcx

    pop rbx
    leave
    ret

;; Print expected range
;; rdi - expected
;; rsi - assert point
;; rdx - size
assert_str_range_expected:
    push rbp
    mov rbp, rsp

    push rdx ;; size
    push rdi ;; range
    push rsi ;; assert_offset

    CCALL .print, [assert_str_build_hex] ;; Hex
    CCALL assert_str_pad, ' ', 4
    CCALL .print, [assert_str_build] ;; Verbatim
    ASSERT_STR_LITERAL ASSERT_NL

    leave
    ret

;; Subroutine that defines a template for expected range
.print:
    label .outer_scope   at rbp+8+8        ;; skip rbp, ret
    label .assert_offset at .outer_scope   ;; unused!
    label .range_ptr     at .outer_scope+8
    label .size          at .outer_scope+16

    push rbp
    mov rbp, rsp

    ;; call print_func
    mov rax, rdi
    CCALL rax, *[.range_ptr], *[.size]

    leave
    ret

;; Converts an operation index to string representation
;; onto the 'assert_str'.
;; rdi - operation index
assert_str_op:
    ;; NOTE NOT PIC: reg + symbol encodes symbol as 32S
    ;; movzx rsi, byte [rdi + .signs.sizes]
    ;; lea rdi, [.signs+(rdi*8)]
    ;; mov rdi, [dword edi]

    ;; PIC complient:
    lea rax, [op_to_str_table.sizes]  ;; load table base
    add rax, rdi
    movzx rsi, byte [rax]

    lea rax, [op_to_str_table.signs]
    lea rdx, [rax + rdi*8]
    mov rdi, [rdx]
    add rdi, rax

    CCALL assert_str_build, rdi, rsi
    ret

;; Converts a range operation index to string representation
;; onto the 'assert_str'.
;; rdi - operation index
assert_str_ranges_op:
    ;; NOTE NOT PIC: reg + symbol encodes symbol as 32S
    ;; movzx rsi, byte [rdi + .signs.sizes]
    ;; lea rdi, [.signs+(rdi*8)]
    ;; mov rdi, [dword edi]

    ;; PIC complient:
    lea rax, [range_op_to_str_table.sizes]  ;; load table base
    add rax, rdi
    movzx rsi, byte [rax]

    lea rax, [range_op_to_str_table.signs]
    lea rdx, [rax + rdi*8]
    mov rdi, [rdx]
    add rdi, rax

    CCALL assert_str_build, rdi, rsi
    ret

RODATA_SEGMENT
op_to_str_table:
.signs:
    dq assert_eq - .signs
    dq assert_ne - .signs
    dq assert_gt - .signs
    dq assert_lt - .signs
    dq assert_ge - .signs
    dq assert_le - .signs
    dq assert_zero - .signs
    dq assert_not_zero - .signs
    dq assert_positive - .signs
    dq assert_negative - .signs
.sizes:
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

range_op_to_str_table:
.signs:
    dq assert_ranges_all_eq - .signs
    dq assert_ranges_any_ne - .signs
    dq assert_ranges_any_eq - .signs
    dq assert_ranges_all_ne - .signs
.sizes:
    db assert_ranges_all_eq_len
    db assert_ranges_any_ne_len
    db assert_ranges_any_eq_len
    db assert_ranges_all_ne_len

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

assert_ranges_all_eq db "All equal "
assert_ranges_all_eq_len = $ - assert_ranges_all_eq
assert_ranges_any_ne db "Any different "
assert_ranges_any_ne_len = $ - assert_ranges_any_ne
assert_ranges_any_eq db "Any equal "
assert_ranges_any_eq_len = $ - assert_ranges_any_eq
assert_ranges_all_ne db "All not equal "
assert_ranges_all_ne_len = $ - assert_ranges_all_ne

BSS_SEGMENT
assert_str_index rq 1
assert_str rb ASSERT_STR_LEN

END_DEBUG
 } ;; __IMPLEMENTATION__


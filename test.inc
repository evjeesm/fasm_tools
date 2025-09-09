include 'comptime.inc'

macro TO_STR dest,src { ITOA dest,src,64,10,0,LOWER }

macro STORE_ARG dest, arg
 {
 local ..text
  virtual at 0
    ..text:: db `arg
    ..text_len = $
  end virtual
  local n,d
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
 }

;; OPS: EQ, NE, GT, LT, LE, GE
macro _ASSERT_ bufsz, reg_1, reg_2, _test, _op, value, expected, msg
 {
  DEBUG
  local .ok
    pushfq
    push reg_1
    push reg_2

    mov reg_1, value
    mov reg_2, expected

    _test reg_1, reg_2

  match =EQ, _op \{ je  .ok \}
  match =NE, _op \{ jne .ok \}

    sub rsp, (bufsz+8)*2 ; allocate string buffers
    STORE_ARG assert_expected, expected
    STORE_ARG assert_value, value

    TO_STR [rsp+8], rax
    mov [rsp], rdx

    TO_STR [rsp+(bufsz+8)+8], r10
    mov [rsp+(bufsz+8)], rdx

    ;; ERROR [assert_m], assert_m_len
    CCALL assert_str_begin
    CCALL assert_str_build, [assert_m], assert_m_len

    ;; ERROR [assert_value], 32
    CCALL assert_str_build, [assert_value], 32

  ;; match =EQ, _op \{ ERROR [assert_eq], assert_eq_len \}
  ;; match =NE, _op \{ ERROR [assert_ne], assert_ne_len \}
  match =EQ, _op \{ CCALL assert_str_build, [assert_eq], assert_eq_len \}
  match =NE, _op \{ CCALL assert_str_build, [assert_ne], assert_ne_len \}

    ;; ERROR [assert_expected], 32
    CCALL assert_str_build, [assert_expected], 32

    ;; ERROR [assert_comma], 2
    CCALL assert_str_build, [assert_comma], 2
    ;; ERROR [assert_obr], 1
    CCALL assert_str_build, [assert_obr], 1

    ;; ERROR [rsp+8], [rsp]
    ;; mov rsi, [rsp]
    CCALL assert_str_build, [rsp+8], *[rsp]

  ;; match =EQ, _op \{ ERROR [assert_eq], assert_eq_len \}
  ;; match =NE, _op \{ ERROR [assert_ne], assert_ne_len \}
  match =EQ, _op \{ CCALL assert_str_build, [assert_eq], assert_eq_len \}
  match =NE, _op \{ CCALL assert_str_build, [assert_ne], assert_ne_len \}

    ;; ERROR [rsp+(bufsz+8)+8], [rsp+(bufsz+8)]

    ;; mov rsi, [rsp+(bufsz+8)]
    CCALL assert_str_build, [rsp+(bufsz+8)+8], *[rsp+(bufsz+8)]

    ;; ERROR [assert_cbr], 1
    CCALL assert_str_build, [assert_cbr], 1

    ;; ERROR [assert_space], 1
    CCALL assert_str_build, [assert_space], 1

    ;; ERROR_INL msg
    ERROR [assert_str], [assert_str_index]

    add rsp, (bufsz+8)*2 ; restore stack
    SYSCALL EXIT, -1
.ok:
    pop reg_2
    pop reg_1
    popfq
  END_DEBUG
 }

macro _ASSERT_64 [...]   ;; _test, _op, value, expected, msg
 {
 common
    _ASSERT_ 16, rax, r10, ...
 }

macro ASSERT_EQ value, expected, msg
 {
    _ASSERT_64 cmp, EQ, value, expected, msg
 }

macro ASSERT_NE value, expected, msg
 {
    _ASSERT_64 cmp, NE, value, expected, msg
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

  segment readable
 assert_m db "[ASSERT] "
 assert_m_len = $ - assert_m

 assert_eq db " == "
 assert_eq_len = $ - assert_eq

 assert_ne db " != "
 assert_ne_len = $ - assert_ne

 assert_gt db " > "
 assert_gt_len = $ - assert_gt

 assert_lt db " < "
 assert_lt_len = $ - assert_lt

 assert_ge db " >= "
 assert_ge_len = $ - assert_ge

 assert_le db " <= "
 assert_le_len = $ - assert_le

 assert_comma db ","
 assert_space db " "

 assert_obr db "("
 assert_cbr db ")"

  segment readable writable
 assert_expected rb 32
 assert_value rb 32

 assert_str_index dq 0
 assert_str rb 1024
END_DEBUG
 }


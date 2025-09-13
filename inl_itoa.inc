include 'regs.inc'

;; Registers in use:
;;   rdi - dest
;;   rdx - char counter
;;   ... - rest registers used in _ITOA_BODY[_SIGNED]
macro ITOA dest*, src*, bits:64, base:10, signed:0, case:LOWER, term
 {
    _ITOA_ASSERT bits,base,signed,case,term
    _ITOA_SET_DEST_AND_SRC dest,src,bits
  if signed = 0
    _ITOA_BODY bits,base
  else
    _ITOA_BODY_SIGNED bits,base
  end if
  if ~ term eq
    mov byte [rdi], term
    inc rdx ;; increment to include terminator character
  end if
 }

macro _ITOA_ASSERT bits, base, signed, case, term
 {
  assert bits >= 1 & bits <= 64 ; bit range exceeded
  assert base <= 36             ; max allowed base exceeded
  assert signed in <0,1>        ; signed value out of range
  assert case in <LOWER,UPPER>  ; letter case for gt 10 bases
  if ~ term eq
    virtual at 0
      db term
      assert $ = 1              ; term is a single character
    end virtual
  end if
 }

;; Registers in use:
;;   rax - src
;;   rdi - dest
;;   ... - rest registers used in _READ_BITS
macro _ITOA_SET_DEST_AND_SRC dest, src, bits
 {
  if ~ dest eq rdi
    lea rdi, dest ; setup destination
  end if
  if src in <X32_REGS>
    mov eax, src
  else if src in <X16_REGS,X8_REGS>
    movzx eax, src
  else ; dereference
    repeat 1
      match [loc], src
      \{
          _READ_BITS [loc], bits
      break
      \}
      if ~ src eqtype rax   \; is not a register
       & ~ src relativeto 0 \; is not numeric const
       & defined src         ; is defined symbol
          lea rax, [src]
      else
          mov rax, src
      end if
    end repeat
  end if
 }

;; Load memory into rax depending on bits amount
;; Registers in use:
;;   rax - src
macro _READ_BITS src, bits
 {
  if bits <= 8
    mov al, byte src
  else if bits <= 16
    mov ax, word src
  else if bits <= 32
    mov eax, dword src
  else if bits <= 64
    mov rax, qword src
  end if
 }

;; Registers in use:
;;   rax - src
;;   rdi - dest
;;   rdx - char counter
;;   ... - rest registers used in _ITOA_BODY
macro _ITOA_BODY_SIGNED bits, base
 {
    push rax ;; save original number
    bt rax, bits-1
    jnc @f   ;; skip positive
    neg rax
    mov byte [rdi], '-'
    inc rdi
@@:
    _ITOA_BODY bits,base
    pop rax
    bt rax, bits-1
    jnc @f   ;; skip positive
    inc rdx
@@:
 }

;; Registers in use:
;;   rax - argument
;;   rdx - mask; remainder of division; ascii char
;;   rcx - counter; copy string
;;   r9  - ascii table
;;   r11 - division operand
;;   rsi - copy string
macro _ITOA_BODY bits, base
 {
  local .digit, .S
    jmp @f
.S: _EMBED_ASCII base, case
@@:
    xor rcx, rcx
  if bits < 64    ; mask selected bits
    mov rdx, 1
    shl rdx, bits
    dec rdx
    and rax, rdx ; apply mask
  end if
    xor edx, edx ; clear remainder
    mov r11, base
    dec rsp
    mov byte [rsp], 0
    inc rcx
    lea r9, [.S]
.digit:
    div r11
    dec rsp             ; allocate digit
    inc cl              ; count bytes
    mov dl, [r9 + rdx]  ; to ascii
    mov byte [rsp], dl  ; store
    xor edx, edx
    test eax, eax
    jnz .digit

    ;; copy from stack to dest
    mov rdx, rcx ; backup count
    cld
    mov rax, 0
    mov rsi, rsp
    rep movsb

    ;; clear stack
    add rsp, rdx
 }

macro _EMBED_ASCII base*, case*
 {
  repeat base
    local ascii
    load ascii byte from ascii_digits:%-1
    if ascii > '9' & case eq LOWER
        ascii = ascii + 32
    end if
    db ascii
  end repeat
 }

virtual at 0
ascii_digits:: db "0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZ"
end virtual

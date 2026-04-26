include '../arch/x86/registers.inc'

;; Registers in use:
;;   edi - dest
;;   edx - char counter
;;   ... - rest registers used in _ITOA_BODY[_SIGNED]
purge ITOA
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
    mov byte [edi], term
    inc edx ;; increment to include terminator character
  end if
 }

purge _ITOA_ASSERT
macro _ITOA_ASSERT bits*, base*, signed*, case*, term
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
;;   eax - src
;;   edi - dest
;;   ... - rest registers used in _READ_BITS
purge _ITOA_SET_DEST_AND_SRC
macro _ITOA_SET_DEST_AND_SRC dest*, src*, bits*
 {
  if ~ dest eq edi
    lea edi, dest ; setup destination
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
      if ~ src eqtype eax   \; is not a register
       & ~ src relativeto 0 \; is not numeric const
       & defined src         ; is defined symbol
        lea eax, [src]
      else
        mov eax, src
      end if
    end repeat
  end if
 }

;; Load memory into eax depending on bits amount
;; Registers in use:
;;   eax - src
purge _READ_BITS
macro _READ_BITS src*, bits*
 {
  if bits <= 8
    mov al, byte src
  else if bits <= 16
    mov ax, word src
  else if bits <= 32
    mov eax, dword src
  end if
 }

;; Registers in use:
;;   eax - src
;;   edi - dest
;;   edx - char counter
;;   ... - rest registers used in _ITOA_BODY
purge _ITOA_BODY_SIGNED
macro _ITOA_BODY_SIGNED bits*, base*
 {
  local .positive, .exit
    push eax ;; save original number
    bt eax, bits-1
    jnc .positive  ;; skip sign
    neg eax
    mov byte [edi], '-'
    inc edi
.positive:
    _ITOA_BODY bits, base
    pop eax
    bt eax, bits-1
    jnc .exit ;; skip sign
    inc edx
.exit:
 }

;; Registers in use:
;;   eax - argument
;;   edx - mask; remainder of division; ascii char
;;   ecx - counter; copy string
;;   esi  - ascii table; copy string
;;   ebx - division operand
purge _ITOA_BODY
macro _ITOA_BODY bits*, base*
 {
  local .code, .digit, .ascii
    jmp .code
.ascii: _EMBED_ASCII base, case
.code:
    xor ecx, ecx
  if bits < 32    ; mask selected bits
    mov edx, 1
    shl edx, bits
    dec edx
    and eax, edx ; apply mask
  end if
    xor edx, edx ; clear remainder
    mov ebx, base
    lea esi, [.ascii]
.digit:
    div ebx
    dec esp             ; allocate digit
    inc cl              ; count bytes
    mov dl, [esi + edx] ; to ascii
    mov byte [esp], dl  ; store
    xor edx, edx
    test eax, eax
    jnz .digit

  ;; Pad extra zeroes if base equals 2^n, n >= 1
  if base in <2,4,8,16,32>
    _PAD_WITH_ZEROES bits, base
  end if

    ;; copy from stack to dest
    mov edx, ecx ; backup count
    cld
    ;; xor eax, eax ;; ? wtf
    mov esi, esp
    rep movsb

    ;; clear stack
    add esp, edx
 }

purge _EMBED_ASCII
macro _EMBED_ASCII base*, case*
 {
  local ..ascii_digits
  virtual at 0
    ..ascii_digits:: db "0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZ"
  end virtual
  repeat base
    local ascii
    load ascii byte from ..ascii_digits:%-1
    if ascii > '9' & case eq LOWER
      ascii = ascii + 32
    end if
    db ascii
  end repeat
 }

purge _PAD_WITH_ZEROES
macro _PAD_WITH_ZEROES bits*, base*
 {
    local .zeroes, .no_zeroes, base_power, tdigits

    ;; log2(base)
    base_power = bsf base
    ;; ceil(bits / base_power)
    tdigits = (bits+base_power-1) / base_power

    mov eax, tdigits
    sub eax, ecx
    jz .no_zeroes
    mov ecx, eax
.zeroes:
    dec esp
    mov byte [esp], '0'
    loop .zeroes
.no_zeroes:
    mov ecx, tdigits
 }

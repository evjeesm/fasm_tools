X64_REGS equ rax,rbx,rdx,rcx,rsi,rdi,rsp,rbp,\
             r8,r9,r10,r11,r12,r13,r14,r15

X32_REGS equ eax,ebx,edx,ecx,esi,edi,esp,ebp,\
             r8d,r9d,r10d,r11d,r12d,r13d,r14d,r15d

X16_REGS equ ax,bx,dx,cx,si,di,sp,bp,\
             r8w,r9w,r10w,r11w,r12w,r13w,r14w,r15w

X8_REGS  equ al,ah,bl,bh,dl,dh,cl,ch,sil,dil,spl,bpl,\
             r8b,r9b,r10b,r11b,r12b,r13b,r14b,r15b

virtual at 0
ascii_digits:: db "0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZ"
end virtual


macro ITOA dest*, src*, bits:64, base:10, signed:0, case:case@LOWER
 {
 assert bits >= 1 & bits <= 64 ; bit range exceeded
 assert base <= 36             ; max allowed base exceeded
 assert signed in <0,1>        ; signed value out of range
 local .digit, .S
    jmp @f
 .S:
 repeat base
 local _ascii_
    load _ascii_ byte from ascii_digits:%-1
    if _ascii_ > '9' & case eq case@LOWER
        _ascii_ = _ascii_ + 32
    end if
    db _ascii_
 end repeat
 @@:
    lea rdi, dest ; setup destination
 if src in <X32_REGS>
    mov eax, src
 else if src in <X16_REGS,X8_REGS>
    movzx eax, src
 else ; dereference
    repeat 1
    match [loc], src
    \{
        mov rax, [loc]
    break
    \}
    if ~ src eqtype rax   \; is not a register
     & ~ src relativeto 0 \; is not numeric const
     & defined src         ; is defined symbol
        display "defined!"
        lea rax, [src]
    else
        mov rax, src
    end if
    end repeat
 end if
    xor rcx, rcx
 if signed = 1
    bt rax, bits-1
    jnc @f
    neg rax
    mov byte [rdi], '-'
    inc rcx
    inc rdi
 @@:
 end if
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

    ; copy from stack to dest
    mov rdx, rcx ; backup count
    cld
    mov rax, 0
    mov rsi, rsp
    rep movsb

    mov byte [rdi], 0 ; null termination
    ; clear stack
    add rsp, rdx
 }

macro ITOA_ST src*, bits:64, base:10, signed:0
 {
 assert bits >= 1 & bits <= 64 ; bit range exceeded
 assert base <= 36             ; max allowed base exceeded
 assert signed in <0,1>        ; signed value out of range
 local .digit, .S, _ascii_
    jmp @f
 .S:
 repeat base
    load _ascii_ byte from ascii_digits:%-1
    db _ascii_
 end repeat
 @@:
 if src in <X32_REGS>
    mov eax, src
 else if src in <X16_REGS,X8_REGS>
    movzx eax, src
 else ; dereference
    repeat 1
    match [loc], src
    \{
        lea rax, [loc]
        break
    \}
    mov rax, src
    end repeat end if
 if signed = 1
    xor r10b, r10b
    bt rax, bits-1
    jnc @f
    neg rax
    dec r10b
 @@:
 end if
 if bits < 64
    ; make bit mask
    mov r8, 1
    shl r8, bits
    dec r8
    ; apply mask
    and rax, r8
 end if
    ; clear remainder
    xor edx, edx
    xor rcx, rcx
    mov r11, base
    push rbx
    mov rbx, rsp
    dec rsp
    mov byte [rsp], 0
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
 if signed = 1
    test r10b, r10b
    jz @f
    dec rsp
    mov byte [rsp], '-'
    inc cl
 @@:
 end if
    mov rsi, rsp
    ALIGN_STACK
 }

macro ALIGN_STACK
 {
    and rsp, -16
 }

macro POP_ST
 {
    mov rsp, rbx
    pop rbx
 }

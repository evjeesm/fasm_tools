macro TO_HEX
 {
    jmp @f
    .hex_table: db '0123456789ABCDEF'
    @@:
    xor rax, rax    ; use as str index
    shr rsi, 2      ; div by 4
    add rdx, rsi
    .nibble:
        ; get nibble
        lea r9, [.hex_table]
        mov bl, dil
        and bl, 0x0f
        add r9b, bl
        mov bl, [r9]
        dec rdx
        mov byte [rdx], bl
        shr rdi, 4
        inc rax
        cmp rax, rsi
        jl .nibble
    ret
 }


; Converts _unsigned_ 64bit register value
; to a textual representation on the stack.
; Stack will be aligned to 16 bytes.
; POP_ST should be used afterwards to restore stack.
;
; Aftermath of the call:
;  rcx - register will hold number of bytes string takes
;  rbx - will hold rsp-8 (rbx is preserved between calls)
;  rsi - will hold head of the string
macro U64_10_TOA_ST reg*
 {
 local .digit
    mov rax, reg
    xor rcx, rcx
    xor rdx, rdx
    mov r11, 10
    push rbx
    mov rbx, rsp
    dec rsp
    mov byte [rsp], 0
    .digit:
        div r11
        dec rsp          ; allocate digit
        inc cl              ; count bytes
        add rdx, '0'        ; to ascii
        mov byte [rsp], dl  ; store
        xor edx, edx
        test eax, eax
        jnz .digit
    mov rsi, rsp
    ALIGN_STACK
 }

macro U8_10_TOA_ST reg*
 {
 local .digit
    mov al, reg
    xor rcx, rcx
    mov r11b, 10
    push rbx
    mov rbx, rsp
    dec rsp
    mov byte [rsp], 0
    .digit:
        div r11b
        dec rsp          ; allocate digit
        inc cl              ; count bytes
        add ah, '0'         ; to ascii
        mov byte [rsp], ah  ; store
        xor ah, ah
        test ax, ax
        jnz .digit
    mov rsi, rsp
    ALIGN_STACK
 }

 macro U64_BASE_TOA_ST reg*, base:10
 {
 local .digit, .S
    jmp @f
        .S db "0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZ"
    @@:
    mov rax, reg
    xor rcx, rcx
    xor rdx, rdx
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
    mov rsi, rsp
    ALIGN_STACK
 }

; Converts _signed_ 64bit register value
; to a textual representation on the stack.
; Stack will be aligned to 16 bytes.
; POP_ST should be used afterwards to restore stack.
;
; Aftermath of the call:
;  rcx - register will hold number of bytes string takes
;  rbx - will hold rsp-8 (rbx is preserved between calls)
;  rsi - will hold head of the string
macro S64_10_TOA_ST reg*
 {
 local .digit, .no_sign
    mov rax, reg
    xor rcx, rcx
    xor rdx, rdx
    mov r11, 10
    xor r10b, r10b
    push rbx
    mov rbx, rsp
    dec rsp
    mov byte [rsp], 0
    test rax, rax
    jns .digit
    neg rax
    inc cl
    dec r10b ; set sign
    .digit:
        div r11
        dec rsp          ; allocate digit
        inc cl              ; count bytes
        add rdx, '0'        ; to ascii
        mov byte [rsp], dl  ; store
        xor edx, edx
        test eax, eax
        jnz .digit
    test r10b,r10b
    jns .no_sign
    dec rsp
    mov byte [rsp], '-'
    .no_sign:
    mov rsi, rsp
    ALIGN_STACK
 }


macro S32_10_TOA_ST reg*
 {
 local .digit, .no_sign
    mov eax, reg
    xor rcx, rcx
    xor edx, edx
    mov r11d, 10
    xor r10b, r10b
    push rbx
    mov rbx, rsp
    dec rsp
    mov byte [rsp], 0
    test eax, eax
    jns .digit
    neg eax
    inc cl
    dec r10b ; set sign
    .digit:
        div r11w
        dec rsp          ; allocate digit
        inc cl              ; count bytes
        add edx, '0'        ; to ascii
        mov byte [rsp], dl  ; store
        xor edx, edx
        test eax, eax
        jnz .digit
    test r10b,r10b
    jns .no_sign
    dec rsp
    mov byte [rsp], '-'
    .no_sign:
    mov rsi, rsp
    ALIGN_STACK
 }


macro S16_10_TOA_ST reg*
 {
 local .digit, .no_sign
    mov ax, reg
    xor rcx, rcx
    xor dx, dx
    mov r11w, 10
    xor r10b, r10b
    push rbx
    mov rbx, rsp
    dec rsp
    mov byte [rsp], 0
    test ax, ax
    jns .digit
    neg ax
    inc cl
    dec r10b ; set sign
    .digit:
        div r11w
        dec rsp          ; allocate digit
        inc cl              ; count bytes
        add dx, '0'         ; to ascii
        mov byte [rsp], dl  ; store
        xor dx, dx
        test ax, ax
        jnz .digit
    test r10b,r10b
    jns .no_sign
    dec rsp
    mov byte [rsp], '-'
    .no_sign:
    mov rsi, rsp
    ALIGN_STACK
 }


macro S8_10_TOA_ST reg*
 {
 local .digit, .no_sign
    movzx eax, reg
    xor rcx, rcx
    mov r11b, 10
    xor r10b, r10b
    push rbx
    mov rbx, rsp
    dec rsp
    mov byte [rsp], 0
    test al, al
    jns .digit
    neg al
    inc cl
    dec r10b ; set sign
    .digit:
        div r11b
        dec rsp          ; allocate digit
        inc cl              ; count bytes
        add ah, '0'         ; to ascii
        mov byte [rsp], ah  ; store
        xor ah, ah
        test al, al
        jnz .digit
    test r10b,r10b
    jns .no_sign
    dec rsp
    mov byte [rsp], '-'
    .no_sign:
    mov rsi, rsp
    ALIGN_STACK
 }


macro S_10_TOA_ST reg*
 {
 local .digit, .no_sign
    mov al, reg
    xor rcx, rcx
    mov r11b, 10
    xor r10b, r10b
    push rbx
    mov rbx, rsp
    dec rsp
    mov byte [rsp], 0
    test al, al
    jns .digit
    neg al
    inc cl
    dec r10b ; set sign
    .digit:
        div r11b
        dec rsp          ; allocate digit
        inc cl              ; count bytes
        add ah, '0'         ; to ascii
        mov byte [rsp], ah  ; store
        xor ah, ah
        test al, al
        jnz .digit
    test r10b,r10b
    jns .no_sign
    dec rsp
    mov byte [rsp], '-'
    .no_sign:
    mov rsi, rsp
    ALIGN_STACK
 }


macro S64_BASE_TOA_ST reg*, base:10
 {
 local .digit, .no_sign, .S
    jmp @f
        .S db "0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZ"
    @@:
    mov rcx, 0
    mov rax, reg
    xor rdx, rdx
    mov r11, base
    xor r10b, r10b
    push rbx
    mov rbx, rsp
    dec rsp
    mov byte [rsp], 0
    test rax, rax
    jns .digit
    neg rax
    inc rcx
    dec r10b ; set sign
    lea r9, [.S]
    .digit:
        div r11
        dec rsp          ; allocate digit
        inc rcx             ; count bytes
        mov dl, [r9 + rdx]  ; to ascii
        mov byte [rsp], dl  ; store
        xor edx, edx
        test eax, eax
        jnz .digit
    test r10b,r10b
    jns .no_sign
    dec rsp
    mov byte [rsp], '-'
    .no_sign:
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


; converts integer value to hex
; rdi -> value to convert 4 .. 64 bits
; rsi -> amount of bits 4, 8, 16, 32, 64
; rdx -> destination char buffer
; ---
; rax <- amount of bytes written to output buf
to_hex:
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
        sub rdx, 1
        mov byte [rdx], bl
        shr rdi, 4
        add rax, 1
        cmp rax, rsi
        jl .nibble
    ret


WRITE  = 1
STDOUT = 1
macro PRINT buf,len
 {
  mov rax, WRITE
  mov rdi, STDOUT
  lea rsi, [buf]
  mov rdx, len
  syscall
 }

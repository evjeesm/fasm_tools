
;;
macro CSTR_LEN reg*, cstr*
 {
    push rdi
    xor rax, rax
    lea rdi, cstr
    mov rcx, -1
    cld
    repnz scasb
    not rcx
    dec rcx
    mov reg, rcx
    pop rdi
 }

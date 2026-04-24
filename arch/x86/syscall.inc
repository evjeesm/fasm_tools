purge SYSCALL
macro SYSCALL nr, [args]
 {
 common
  local register, reg_c, ebp_saved
 common
    reg_c = 6
    register equ ebp
    register equ edi
    register equ esi
    register equ edx
    register equ ecx
    register equ ebx
 forward
  if reg_c
    ebp_saved = 0
    if register eq ebp
      ebp_saved = 1
      push ebp
    end if
    repeat 1
      match *[loc], args \{
        mov register, [loc]
        break
      \}
      match [loc], args \{
        if ~ register eq loc
          lea register, [loc]
        end if
        break
      \}
      match val, args \{
        if val eqtype register    ;; Is a reg
          if ~ val eq register  ;; Is different to arg reg
            mov register, val
          end if
        else
          if val <> 0           ;; non zero
            mov register, val
          else                  ;; zero
            xor register, register
          end if
        end if
      \}
    end repeat
    restore register
    reg_c = reg_c - 1
  end if
 common
    mov eax, nr
    int 0x80
    if ebp_saved
      pop ebp
    end if
 }

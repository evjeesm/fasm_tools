if ~ definite _SYSCALL_INC_
_SYSCALL_INC_ equ 1

macro SYSCALL nr, [args]
 {
common
  local register, reg_c
common
    reg_c = 6
    register equ r9
    register equ r8
    register equ r10
    register equ rdx
    register equ rsi
    register equ rdi
forward
  if reg_c
    repeat 1
      match *[loc], args
      \{
        mov register, [loc]
      break
      \}
      match [loc], args
      \{
        if ~ register eq loc
            lea register, [loc]
        end if
      break
      \}
      match val, args
      \{
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
    mov rax, nr
    syscall
 }

end if ;; _SYSCALL_INC_

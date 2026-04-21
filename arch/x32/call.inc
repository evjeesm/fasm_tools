if ~ definite _CALL_INC_
_CALL_INC_ equ 1

macro CALL func*, [args]
 {
common
  local register, reg_c, count
    count = 0
forward
    count = count + 1
common
    reg_c = 6
    register equ r9d
    register equ r8d
    register equ ecx
    register equ edx
    register equ esi
    register equ edi
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
            if ~ val relativeto 0 | val <> 0           ;; non zero
                mov register, val
            else                  ;; zero
                xor register, register
            end if
        end if
      \}
    end repeat

    restore register
    reg_c = reg_c - 1
    count = count - 1
  end if
reverse
  if count
        push args
    count = count - 1
  end if
common
    call func
 }

end if ;; _CALL_INC_


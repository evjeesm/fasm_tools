
; Mark code block as debug block
; that can be excluded from release build.
; Debug mode is default.
; To compile in release mode: fasm -d RELEASE=1 code.asm

macro DEBUG
 {
 if ~ defined RELEASE
 }

macro END_DEBUG
 {
 end if ; ~ RELEASE
 }

macro DISP_DEC num*
 {
    local c,n,dv,dg
    n = num
    c = 0
    dv = 1
    while n
      n = n/10
      c = c+1
      dv = dv*10
    end while
    n = num
    while dv > 1
      dv = dv/10
      dg = '0' + ((n/dv) mod 10)
      display dg
    end while
 }

macro DISP_HEX num*
 {
    local nib, i, shft, d
    display '0x'
    nib = (bsr num)/4
    i = nib + 1
    while i <> 0
      shft = (i-1)*4
      d = '0' + (num and (0xF shl shft)) shr shft
      i = i-1
      if d > '9'
        d = d+'A'-'9'-1
      end if
      display d
    end while
 }

macro DISP_STR var*
 {
    local s
    s = var
    display '"'
    while s
      display s and 0xFF
      s = s shr 8
    end while
    display '"'
 }

macro FMT [fmt]
 {
 common
    local parse, type, last_val
    define S 0
    define D 1
    define X 2
    macro parse type*, val*
    \{
      if type = S
        DISP_STR val
      else if type = D
        DISP_DEC val
      else if type = X
        DISP_HEX val
      end if
      last_val = val
    \}
    type = S
    irps val, fmt
    \{
      if val eqtype ''
        display val
      else if val in <S,D,X>
        type = val
      else if val eq NL
        display 10
      else
        if val eq .
           assert defined last_val
           val = last_val
        end if
        parse type, val
      end if
    \}
    restore S,D,X
 }

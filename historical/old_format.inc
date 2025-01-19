
macro DISP_S_DEC num*
 {
    local n
    n = num
    if (num and FASM_MAX_INT) <> 0 & (bsr (num and FASM_MAX_INT)) = 63
        display '-'
        n = not num and FASM_MAX_INT + 1
    end if
    DISP_DEC n
 }


macro DISP_DEC num*
 {
    local c,n,dv,dg
    n = num and FASM_MAX_INT
    c = 0
    dv = 1
    if n = 0
      display '0'
    else
      while n > 0
        c = c+1
        if (dv > 1 shl 63)
            display '1'
        else
            dv = dv*10
        end if
        n = n/10
      end while
      n = num and FASM_MAX_INT
      while dv > 1
        dv = dv/10
        dg = '0' + ((n/dv) mod 10)
        display dg
      end while
    end if
 }

macro DISP_HEX num*
 {
    local nib, i, shft, d
    display '0x'
    if num <> 0
        bit = bsr (num and FASM_MAX_INT)
    else
        bit = 0
    end if
    nib = bit/4
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

 macro DISP_S_HEX num*
 {
    local n
    n = num
    if num <> 0 & (bsr (num and FASM_MAX_INT)) = 63
        display '-'
        n = not num and FASM_MAX_INT + 1
    end if
    DISP_HEX n
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


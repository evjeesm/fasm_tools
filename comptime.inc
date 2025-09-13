;; Parse compilation target
if ~ defined RELEASE
  _RELEASE=0 ;; default -> debug mode
else
  _RELEASE=RELEASE
end if

; Mark code block as debug block
; that can be excluded from release build.
; Debug mode is default.
; To compile in release mode: fasm -d RELEASE=1 code.asm
macro DEBUG { if _RELEASE = 0 }
macro END_DEBUG { end if } ; _RELEASE = 0

FASM_MAX_INT equ 0xffff_ffff_ffff_ffff
SIGNED equ 1
UNSIGNED equ 0
PRECISION equ 3

macro DISP_STR var*
 {
    local s,i
    s = var
    i = 0
    display '"'
    while i < 8
        display s and 0xFF
        s = s shr 8
        i = i+1
    end while
    display '"'
 }

macro DISP_DEC num*, sign:UNSIGNED
 {
    local orig,n,dv,dg
    orig = (num) and FASM_MAX_INT
    if sign = 1
        if orig <> 0 & (bsr orig) = 63
            display '-'
            orig = not orig and FASM_MAX_INT + 1
        end if
    end if
    n = orig
    dv = 1
    if n = 0
        display '0'
    else
        while n > 0
            if (dv > 1 shl 63)
                display '1'
            else
                dv = dv*10
            end if
            n = n/10
        end while
        n = orig
        while dv > 1
            dv = dv/10
            dg = '0' + ((n/dv) mod 10)
            display dg
        end while
    end if
 }

macro DISP_HEX num*, sign:UNSIGNED
 {
    local n,nib,i,shft,d
    n = (num) and FASM_MAX_INT
    if sign = 1
        if n <> 0 & (bsr n) = 63
            display '-'
            n = not n and FASM_MAX_INT + 1
        end if
    end if
    display '0x'
    if n <> 0
        bit = bsr n
    else
        bit = 0
    end if
    nib = bit/4
    i = nib + 1
    while i <> 0
        shft = (i-1)*4
        d = '0' + (n and (0xF shl shft)) shr shft
        i = i-1
        if d > '9'
            d = d+'A'-'9'-1
        end if
        display d
    end while
 }

macro DISP_DOUBLE num*, precision:PRECISION
 {
    local bitlen,explen,mantisalen,expmask,expbias,mantisamask
    local sign,exp,actual_exp,norm_mantisa,i,mask
    local integer_part,fractional_part
    bitlen = 64
    explen = 11
    expbias = 1023
    mantisalen = 52
    sign = ((num) shr (bitlen - 1)) and 0x1
    expmask = (0x1 shl explen) - 1
    exp = (num) shr mantisalen and expmask
    actual_exp = exp - expbias
    mantisamask = (0x1 shl mantisalen) - 1
    mantisa = (num) and (mantisamask)
    norm_mantisa = mantisa
    if exp <> -expbias
        norm_mantisa = (mantisa or (1 shl mantisalen))
    end if
    if (exp = expmask & mantisa = 0)
        if sign
            display '-'
        else
            display '+'
        end if
        display 'inf'
    else if (exp = expmask & mantisa <> 0)
        display 'NaN'
    else
        if sign
            display '-'
        end if
        if actual_exp > 52
            display '<TOO LARGE>'
        else if actual_exp < -52
            display '<TOO SMALL>'
        else
            if actual_exp > 0
                i = (bsr norm_mantisa) - actual_exp
                mask = (1 shl i) - 1
                scaled_mantisa = ((norm_mantisa and mask) shl actual_exp)
            else
                scaled_mantisa = norm_mantisa shr -actual_exp
            end if
            integer_part = (norm_mantisa shr (mantisalen - actual_exp))
            fractional_part = scaled_mantisa and (mantisamask)
            DISP_DEC integer_part
            display '.'
            repeat precision
                fractional_part = fractional_part * 10
                DISP_DEC (fractional_part shr mantisalen) mod 10
                fractional_part = fractional_part and mantisamask
            end repeat
        end if
    end if
 }


macro FMT [fmt]
 {
 common
    local parse, last_val, type, sign, precision
    define %S 0
    define %D 1
    define %X 2
    define %F 3
    define %P 4
    macro parse type*, val*, sign*
    \{
        if type = %S
            DISP_STR val
        else if type = %D
            DISP_DEC val, sign
        else if type = %X
            DISP_HEX val, sign
        else if type = %F
            if defined precision
                DISP_DOUBLE val, precision
            else
                DISP_DOUBLE val
            end if
        else if type = %P
            precision = val
        end if
        if sign = SIGNED
            sign = UNSIGNED
        end if
        last_val = val
    \}
    sign = UNSIGNED
    irps val, fmt
    \{
        rept 1 \\{
        if ~ \`val in <'+'> ; not +
            if val eqtype '' ; text
                display val
            else if val in <%S,%D,%X,%F,%P>
                type = val
            else if val eq %NL
                display 10
            else
              if val eq .
                assert defined last_val
                val = last_val
              end if
              parse type, val, sign
            end if
        else ; +
            sign = SIGNED
        end if
        \\}
    \}
    restore %S, %D, %X, %F, %P
 }

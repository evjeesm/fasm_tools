
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

FASM_MAX_INT equ 0xffff_ffff_ffff_ffff
SIGNED equ 1
UNSIGNED equ 0

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

macro DISP_DOUBLE num*, precision:3
 {
    local sign,exp,actual_exp,norm_mantisa,i
    local integer_part,fractional_part,scale,digs,full_frac
    sign = ((b) shr 63) and 1
    exp = (b) shr 52 and 0x7FF
    actual_exp = exp - 1023
    mantisa = (b) and (0xF_FFFF_FFFF_FFFF)
    norm_mantisa = (mantisa or (1 shl 52))
    if (exp = 0x7FF & mantisa = 0)
        if sign
            display '-'
        else
            display '+'
        end if
        display 'inf'
    else if (exp = 0x7FF & mantisa <> 0)
        display 'NaN'
    else
        if sign
            display '-'
        end if
        if actual_exp > 0
            i = (bsr norm_mantisa) - actual_exp
            mask = (1 shl i) - 1
            scaled_mantisa = ((norm_mantisa and mask) shl actual_exp)
        else
            scaled_mantisa = norm_mantisa shr -actual_exp
        end if
        integer_part = (norm_mantisa shr (52 - actual_exp))
        scale = 1
        rept precision
         \{
            scale = scale * 10
         \}
        fractional_part = scaled_mantisa and ((1 shl 52) - 1)
        digs = 0
        full_frac = 0
        while digs < (precision + 1)
            digs = digs + 1
            fractional_part = fractional_part * 10
            full_frac = full_frac * 10 + (fractional_part shr 52)
            fractional_part = fractional_part and ((1 shl 52) - 1)
        end while
        if (full_frac mod 10) >= 5
            full_frac = full_frac + 10 - (full_frac mod 10)
        end if
        full_frac = full_frac / 10
        if full_frac = scale
            full_frac = 0
            integer_part = integer_part + 1
        end if
        while full_frac <> 0 & full_frac mod 10 = 0
            full_frac = full_frac / 10
        end while
        DISP_DEC integer_part,
        display '.'
        DISP_DEC full_frac
    end if
 }


macro FMT [fmt]
 {
 common
    local parse, last_val, type, sign
    define S 0
    define D 1
    define X 2
    macro parse type*, val*, sign*
    \{
        if type = S
            DISP_STR val
        else if type = D
            DISP_DEC val, sign
        else if type = X
            DISP_HEX val, sign
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
            else if val in <S,D,X>
                type = val
            else if val eq NL
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
    restore S, D, X
 }

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

;;
;; Format macro (like printf in C)
;; Use type symbol to set fomatting strategy for all next arguments.
;; Type symbols:
;;  %S - string
;;  %D - signed integer
;;  %U - unsigned integer
;;  %X - hexadecimal (unsigned by default)
;;  %F - double
;;  %P - define a precision for the next double fomatting
;; Dot will be substituted by last argument
;;  (usefull for printing same thing in different formats)
macro FMT [fmt*]
 {
 common
  local parse, last_val, type, negative, precision
  define %S 0 ; string
  define %D 1 ; signed integer
  define %U 2 ; unsigned integer
  define %X 3 ; hexadecimal
  define %F 4 ; double
  define %P 5 ; double precision
  negative = 0
  macro parse type*, val*
  \{
    local _val
    if negative = 1
      _val = -val
    else
      _val = val
    end if
    if type = %S
      DISP_STR val
    else if type = %D
      DISP_DEC _val, SIGNED
    else if type = %U
      DISP_DEC _val, UNSIGNED
    else if type = %X
      DISP_HEX _val
    else if type = %F
      if defined precision
        DISP_DOUBLE _val, precision
      else
        DISP_DOUBLE _val
      end if
    else if type = %P
      precision = val
    end if
    negative = 0
    last_val = _val
  \}
  irps val, fmt
  \{
  rept 1
  \\{
  if ~ \`val in <'+','-'> ; not + or -
    if val eqtype '' ; text
      display val
    else if \`val in <'%S','%D','%U','%X','%F','%P'>
      type = val
    else if val eq %NL
      display 10
    else
      if val eq .
        assert defined last_val
        val = last_val
      end if
      parse type, val
    end if
  else ; + or -
    if \`val eq '-'
      if negative = 1
        negative = 0
      else
        negative = 1
      end if
    end if
  end if
  \\}
  \}
  restore %S, %D, %U, %X, %F, %P
 }

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
    if orig <> 0 & (bsr orig) = (_BIT_LEN - 1)
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
      if (dv > 1 shl (_BIT_LEN - 1))
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

macro DISP_HEX num*
 {
  local n,nib,i,shft,d
  n = (num) and FASM_MAX_INT
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

_MANTISA_LEN = 52
_MANTISA_MASK = (1 shl _MANTISA_LEN) - 1
_EXP_BIAS = 1023
_EXP_LEN = 11
_EXP_MASK = (1 shl _EXP_LEN) - 1
_BIT_LEN = 64

macro DISP_DOUBLE num*, precision:PRECISION
 {
  local n,mantisa,sign,exp,actual_exp

  n = (num)
  sign = (n shr (_BIT_LEN - 1)) and 0x1
  exp = n shr _MANTISA_LEN and _EXP_MASK
  actual_exp = exp - _EXP_BIAS
  mantisa = n and (_MANTISA_MASK)

  if (exp = _EXP_MASK & mantisa = 0)
    _DOUBLE_INF sign
  else if (exp = _EXP_MASK & mantisa <> 0)
    display 'NaN'
  else
    if sign
      display '-'
    end if
    ; handle extreme exponents with scientific notation
    if (actual_exp > _MANTISA_LEN) | (actual_exp < -_MANTISA_LEN)
      _DOUBLE_SCIENTIFIC_NOTATION actual_exp, mantisa, precision
    else
      _DOUBLE_FLOATING_POINT_NOTATION actual_exp, mantisa, precision
    end if
  end if
}

macro _DOUBLE_INF sign*
 {
  if sign
    display '-'
  else
    display '+'
  end if
  display 'inf'
 }

macro _DOUBLE_SCIENTIFIC_NOTATION actual_exp*, mantisa*, precision*
 {
  ; normalized mantissa fraction = norm_mantisa / 2^_MANTISA_LEN
  display '1.'
  ; print fractional bits of mantissa as decimal
  fractional_part = mantisa
  repeat precision
    fractional_part = fractional_part * 10
    DISP_DEC (fractional_part shr _MANTISA_LEN) mod 10
    fractional_part = fractional_part and _MANTISA_MASK
  end repeat

  display ' * 2^('
  if actual_exp < 0
    display '-'
    DISP_DEC -actual_exp
  else
    display '+'
    DISP_DEC actual_exp
  end if
  display ')'
 }

macro _DOUBLE_FLOATING_POINT_NOTATION actual_exp*, mantisa*, precision*
 {
  local scaled_mantisa, integer_part, fractional_part, norm_mantisa
  norm_mantisa = mantisa
  if actual_exp <> 0
    norm_mantisa = (mantisa or (1 shl _MANTISA_LEN))
  end if

  if actual_exp > 0
    local i, mask
    i = (bsr norm_mantisa) - actual_exp
    mask = (1 shl i) - 1
    scaled_mantisa = ((norm_mantisa and mask) shl actual_exp)
  else
    scaled_mantisa = norm_mantisa shr -actual_exp
  end if
  integer_part = (norm_mantisa shr (_MANTISA_LEN - actual_exp))
  fractional_part = scaled_mantisa and (_MANTISA_MASK)
  DISP_DEC integer_part
  display '.'
  repeat precision
    fractional_part = fractional_part * 10
    DISP_DEC (fractional_part shr _MANTISA_LEN) mod 10
    fractional_part = fractional_part and _MANTISA_MASK
  end repeat
 }

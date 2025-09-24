include 'fmt.inc'

;; String literals
FMT 'Hello, '
FMT 'world' %NL

;; String output
FMT %X 0x65_72_65_68_74_20_69_68 ' --> ' %S . %NL

;; Format expressions
FMT_EXPR <mov rax, A>,\
         <mov rbx, A>

;; Integer formats
FMT %X 'Hex: ' -1 ', signed: ' %D . ', unsigned: ' %U . %NL

;; Floating point number format
FLOAT = 0
FMT 'float: ' %F FLOAT %NL
FMT 'float prec(' %P 3 %D . '): ' %F 1.0 %NL
FMT 'float prec(' %P 6 %D . '): ' %F 0.000000001 %NL
FMT 'float prec(' %P 9 %D . '): ' %F 0.00000000000000001 %NL



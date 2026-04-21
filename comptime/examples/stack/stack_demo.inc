include '../../fmt.inc'
include '../../containers/stack.inc'

STACK s, 8

STACK_PUSH s, "hello"
STACK_PUSH s, "world"
STACK_PUSH s, "ok go"

WHILE_NOT_EMPTY s
  STACK_PEEK s, val
  VTEXT_DISPLAY val
  display 0xA
  STACK_POP s
END_WHILE_NOT_EMPTY

VTEXT_STRUCT ..my_struct, <key, db 1>, <value, dw 2>
FMT %D ..my_struct.len %NL
FMT "offset: " %D ..my_struct.key.offset %NL
FMT "size: " %D ..my_struct.key.size %NL

FMT "offset: " %D ..my_struct.value.offset %NL
FMT "size: " %D ..my_struct.value.size %NL

STACK struc_s, ..my_struct.len

STACK_PUSH_VTEXT struc_s, ..my_struct
VTEXT_STRUCT ..my_struct, <key, db 10>, <value, dw 10>
STACK_PUSH_VTEXT struc_s, ..my_struct

WHILE_NOT_EMPTY struc_s
  STACK_PEEK struc_s, ..my_struct
  VTEXT_HEX ..my_struct, ..my_struct.key.offset, ..my_struct.key.size
  display "|"
  VTEXT_HEX ..my_struct, ..my_struct.value.offset, ..my_struct.value.size
  display 0xA
  STACK_POP struc_s
END_WHILE_NOT_EMPTY

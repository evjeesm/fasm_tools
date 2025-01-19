include 'comptime.inc'
include 'iota.inc'

;; override functions

macro IOTA_COUNTER_FUNC
 { IOTA_count = IOTA_count or IOTA_stride }

macro IOTA_STRIDE_FUNC
 { IOTA_stride = IOTA_stride shl 1 }

IOTA A
IOTA B
IOTA C
IOTA D
IOTA E
IOTA F
IOTA G
IOTA H
IOTA I

FMT %X 'A: ' A %NL
FMT %X 'B: ' B %NL
FMT %X 'C: ' C %NL
FMT %X 'D: ' D %NL
FMT %X 'E: ' E %NL
FMT %X 'F: ' F %NL
FMT %X 'G: ' G %NL
FMT %X 'H: ' H %NL
FMT %X 'I: ' I %NL

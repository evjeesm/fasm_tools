;; Escape Sequence
restore ESC
ESC equ ''

;; Control Sequence Introducer
restore CSI
CSI equ ESC, '['

;; Select Graphics Rendition

restore SGR_RED
SGR_RED equ CSI,'31m'

restore SGR_RESET
SGR_RESET equ CSI,'0m'

purge SEQ
macro SEQ [args*]
 {
 forward
  irps a, args
  \{
    db a
  \}
 }

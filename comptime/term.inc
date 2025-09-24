;; Escape Sequence
ESC equ ''

;; Control Sequence Introducer
CSI equ ESC, '['

;; Select Graphics Rendition
SGR_RED equ CSI,'31m'
SGR_RESET equ CSI,'0m'


macro SEQ [args*]
 {
 forward
    irps a, args
    \{
      db a
    \}
 }

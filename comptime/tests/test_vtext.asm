include 'comptime.inc'
include 'vtext.inc'

;; Create vtext.
;; prefix name with '..' in order to prevent
;; VTEXT labels from interfering with the actual assembly
VTEXT ..my_text, "Hello!", 0xA

;; Display vtext at compile time in fasm feed
VTEXT_DISPLAY ..my_text ;, 0, ..my_text.len

;; VTEXT ..my_text, "Oooops" ;; Can't define same vtext twice

;; You can use set command to override vtext with new content
VTEXT_SET ..my_text, 'new value', 0xA
VTEXT_DISPLAY ..my_text

;; Fill region of text with single char or alternating pattern
VTEXT_FILL ..my_text, 4, 5, 'x','_'
VTEXT_DISPLAY ..my_text

;; Assemble structured data into the buffer
VTEXT_RESET ..my_text ;; clear contents of the vtext
VTEXT_ASSEMBLE ..my_text, dq 0xbadcafe, dw 0xbaba, db 0x1
VTEXT_HEX ..my_text
display 0xA

;; Read whole file
VTEXT_FILE this, __FILE__
VTEXT_DISPLAY this

include 'comptime.inc'
include 'vtext.inc'

;; Create vtext with a name prefixed by '..'
;; this is important when VTEXT in the assembly
;; to not interfere with labeling!
VTEXT ..ara, "Hello!"

;; Append arguments to an existing vtext
VTEXT_APPEND ..ara, ", World"

;; Display vtext at compile time in fasm feed
VTEXT_DISPLAY ..ara ;, 0, ..ara.len

;; VTEXT ..ara, "Oooops" ;; Can't declare same vtext twice, but...

;; You can overwrite contents of the existing vtext
;; its can be done with a help of temporary vtext (@@/@b)
VTEXT @@, "Temporary text"
VTEXT_COPY ..ara, @b
VTEXT_DISPLAY ..ara

;; Read whole file
VTEXT_FILE this, __FILE__
VTEXT_DISPLAY this

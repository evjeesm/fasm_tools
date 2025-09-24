
macro VTEXT name*, [args*]
 {
common
  virtual at 0
    @@:: db args
    name#\.len = $
  end virtual
    name equ @b
 }

macro VTEXT_BAKE dest*, vsrc*, text_len*
 {
  local n, d
  n = 0
  d = 4
  while n < text_len
    repeat (text_len-n)/d
      local _STORE
      macro _STORE dest, src, size, offset
      \{
       local chunk
        load chunk size from src:offset
        mov size [dest+offset], chunk
      \}
      if d = 4
        _STORE dest, vsrc, dword, n
      else if d = 2
        _STORE dest, vsrc, word, n
      else if d = 1
        _STORE dest, vsrc, byte, n
      end if
      n = n + d
    end repeat
    d = d / 2
  end while
 }

;;
;;  VTEXT a, SGR_RED, "HELLO", SGR_RESET
;;  VTEXT_BAKE 0, a, a.len
;;
;;  VTEXT a, SGR_RESET, "FUCK YOU DEAR", SGR_RESET
;;  VTEXT_BAKE 0, a, a.len


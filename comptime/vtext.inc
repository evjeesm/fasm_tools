;;
;; NOTE: Use @@ and @b to make a temporary vtext
;;


;; Creates a text in a virtual address space
macro VTEXT name*, [args*]
 {
 common
  virtual at 0
    name:: db args
    name#.len = $
  end virtual
 }

macro VTEXT_FILE name*, filepath*
 {
  virtual at 0
    name:: file filepath
    name#.len = $
  end virtual
 }

;; Append to a previously defined VTEXT
macro VTEXT_APPEND name*, [args*]
 {
 common
  virtual name
    db args
    name#.len = $
  end virtual
 }

;; Enlarge vtext with additional 'size'
macro VTEXT_EXPAND vsrc*, size*
 {
  virtual vsrc
    rb size
    vsrc#.len = $
  end virtual
 }

;; Generate immediate movs of the virutal text to a destination
macro VTEXT_BAKE dest*, vsrc*
 {
  local n, d, text_len
  n = 0
  d = 4
  text_len = vsrc#.len
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

;; Generate immediate stack pushes of the virtual text
macro VTEXT_BAKE_STACK vsrc*
 {
  local n, d, text_len
  text_len = vsrc#.len
  n = len
  d = 4
  while n > 0
    repeat n / d
      local _STORE
      macro _STORE src, size, offset
      \{
        local chunk
        if size = 8
          load chunk qword from src:offset
          mov rax, chunk
          push rax
        else if size = 4
          load chunk word from src:offset+2
          push word chunk
          load chunk word from src:offset
          push word chunk
        else if size = 2
          load chunk word from src:offset
          push word chunk
        else if size = 1
          load chunk byte from src:offset
          sub rsp, 1
          mov byte [rsp], chunk
        end if
      \}
      _STORE vsrc, d, n-d
      n = n - d
    end repeat
    d = d / 2
  end while
 }

macro VTEXT_SLICE vsrc*, _beg:0, _end:-1
 {
  local stop, b
  if _end eq -1
    stop = vsrc#.len
  else
    stop = _end - _beg
  end if
  repeat stop
    load b byte from vsrc:_beg+%-1
    store byte b at vsrc:%-1
  end repeat
  store byte 0 at vsrc:stop
  vsrc#.len = stop
 }

macro VTEXT_SET vdest*, [args*]
 {
 common
  local tmp, tmp_size
  virtual at 0
    tmp:: db args
    tmp_size = $
  end virtual
  if vdest#.len < tmp_size
    VTEXT_EXPAND vdest, tmp_size - vdest#.len
  end if
  repeat tmp_size
    load b byte from tmp:%-1
    store byte b at vdest:%-1
  end repeat
  if vdest#.len > tmp_size
    store byte 0 at vdest:tmp_size
  end if
  vdest#.len = tmp_size
 }

macro VTEXT_COPY vdest*, vsrc*
 {
  if vdest#.len < vsrc#.len
    VTEXT_EXPAND vdest, vsrc#.len - vdest#.len
  end if
  repeat vsrc#.len
    load b byte from vsrc:%-1
    store byte b at vdest:%-1
  end repeat
  if vdest#.len > vsrc#.len
    store byte 0 at vdest:vsrc#.len
  end if
  vdest#.len = vsrc#.len
 }

macro VTEXT_WRITE vdest*, offset*, vsrc*
 {
  if vdest#.len < offset + vsrc#.len
    VTEXT_EXPAND vdest, offset + vsrc#.len - vdest#.len
  end if
  repeat vsrc#.len
    load b byte from vsrc:%-1
    store byte b at vdest:offset+%-1
  end repeat
 }

macro VTEXT_FMT vsrc*, offset*, size*
 {
  local _READ, total, off, chunk, chunk_sz
  total = vsrc#.len - offset
  if total < 0
    total = 0
  else if total > size
    total = size
  end if
  chunk_sz = 8
  off = offset
  macro _READ chunk \{ load chunk byte from vsrc:off \}
  macro _READ chunk \{ load chunk word from vsrc:off \}
  macro _READ chunk \{ load chunk dword from vsrc:off\}
  macro _READ chunk \{ load chunk qword from vsrc:off\}
  while chunk_sz > 0
    repeat total / chunk_sz
      _READ chunk
      FMT %R chunk
      off= off + chunk_sz
      total = total - chunk_sz
    end repeat
    chunk_sz = chunk_sz / 2
    purge _READ
  end while
  FMT %NL
 }

macro VTEXT_LOAD_CHAR vsrc*, index*, output*
 {
    load output byte from vsrc:index
 }

macro VTEXT_FILL vsrc*, char*, _beg:0, _end:-1
{
  local stop
  if _end eq -1
    stop = vsrc#.len
  else
    stop = _end - _beg
  end if
  repeat stop
    store char at vsrc:_beg+%-1
  end repeat
 }

macro VTEXT_XOR vsrc*, key*
 {
  local b
  repeat vsrc#.len
    load b byte from vsrc:%-1
    store byte b xor key at vsrc:%-1
  end repeat
 }
;;
;;  VTEXT a, SGR_RED, "HELLO", SGR_RESET
;;  VTEXT_BAKE 0, a, a.len
;;
;;  VTEXT a, SGR_RESET, "FUCK YOU DEAR", SGR_RESET
;;  VTEXT_BAKE 0, a, a.len


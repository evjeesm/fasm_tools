if ~ definite _VTEXT_INC_
_VTEXT_INC_ equ 1
;;
;; NOTE: Use @@ and @b to make a temporary vtext
;;


;; Creates a text in a virtual address space
macro VTEXT name*, [arg]
 {
 common
  virtual at 0
    name::
    if ~ arg eq
        db arg
    end if
    if `name eq '@@' ;; TMP VTEXT
      @b#.len = $
    else
      name#.len = $
    end if
  end virtual
 }

macro VTEXT_FILE name*, filepath*
 {
  virtual at 0
    name:: file filepath
    if `name eq '@@' ;; TMP VTEXT
      @b#.len = $
    else
      name#.len = $
    end if
  end virtual
 }

;; Append to a previously defined VTEXT
macro VTEXT_APPEND name*, [arg*]
 {
 common
  virtual name
    db arg
    name#.len = $
  end virtual
 }

;; Append to a previously defined VTEXT
macro VTEXT_APPEND_AS_IS name*, [arg*]
 {
 forward
  irps a, arg
  \{
    VTEXT_APPEND name, \`a
  \}
 }


;; Enlarge vtext with additional 'size'
macro VTEXT_EXPAND name*, size*
 {
  virtual name
    rb size
    name#.len = $
  end virtual
 }

;; Generic bake that takes user baking strategy.
;; Param BAKE_CHUNK - is a macro of form:
;;   `BAKE_CHUNK dest, vsrc, size, offset`
;;
macro VTEXT_BAKE dest*, vsrc*, BAKE_CHUNK*, until:?, init_offset:0, init_chunk_sz:8
 {
  local offset, chunk_sz, _until
  offset = init_offset
  chunk_sz = init_chunk_sz

  if until eq ?
    _until = vsrc#.len
  else
    _until = until
  end if
  STATIC_ASSERT _until <= vsrc#.len, "Size should not exceed vtext bounds!"

  while offset < _until
    repeat (_until - offset) / chunk_sz
      BAKE_CHUNK dest, vsrc, chunk_sz, offset
      offset = offset + chunk_sz
    end repeat
    chunk_sz = chunk_sz / 2
  end while
 }

;; Generate immediate movs of the virutal text to a destination
macro VTEXT_BAKE_MOVS dest*, vsrc*
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

macro VTEXT_SET vdest*, [arg*]
 {
 common
  local tmp, tmp_size
  virtual at 0
    tmp:: db arg
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

macro VTEXT_READ vdest*, vsrc*, vsrc_offset*, size*
 {
  STATIC_ASSERT vsrc_offset + size <= vsrc#.len, "Out of vsrc bounds"
  if vdest#.len < vsrc#.len - vsrc_offset
    VTEXT_EXPAND vdest, vsrc#.len - vsrc_offset - vdest#.len
  end if
  repeat size
    load b byte from vsrc:vsrc_offset+%-1
    store byte b at vdest:%-1
  end repeat
  vdest#.len = size
 }

macro VTEXT_DISPLAY vsrc*, offset:0, until:?
 {
  local _PRINT, _until
  macro _PRINT dest, src, _size, _offset
  \{
    local chunk
    if _size = 8
      load chunk qword from src:_offset
    else if _size = 4
      load chunk dword from src:_offset
    else if _size = 2
      load chunk word from src:_offset
    else if _size = 1
      load chunk byte from src:_offset
    end if
    STATIC_ASSERT _size <> 0, "Chunk size should never be zero!"
    repeat _size
      display (chunk shr (8*(%-1))) and 0xFF
    end repeat
  \}

  if until eq ?
    _until = vsrc#.len
  else
    _until = until
  end if

  VTEXT_BAKE _, vsrc, _PRINT, _until, offset
 }

macro VTEXT_DISPLAY_NULLTERM vsrc*, offset:0
 {
    local char,_offset
    _offset = offset
    while 1
      VTEXT_LOAD_CHAR vsrc, _offset, char
      if char = 0
          break
      end if
      display char and 0xFF
      _offset = _offset + 1
    end while
 }

macro VTEXT_HEX vsrc*, offset:0, until:?
 {
  local _PRINT, _until
  macro _PRINT dest, src, _size, _offset
  \{
    local chunk
     load chunk byte from src:_offset
     if chunk < 16
         FMT '0'
     end if
     FMT %X chunk
  \}

  if until eq ?
    _until = vsrc#.len
  else
    _until = until
  end if

  VTEXT_BAKE _, vsrc, _PRINT, _until, offset, 1
 }

macro VTEXT_LOAD_CHAR vsrc*, offset*, output*
 {
  load output byte from vsrc:offset
 }

macro VTEXT_STORE_CHAR vdest*, offset*, char*
 {
  store byte char at vdest:offset
 }

macro VTEXT_READ_FIELD vsrc*, offset*, size*, output*
 {
  if size = 1
   load output byte from vsrc:offset
  else if size = 2
   load output word from vsrc:offset
  else if size = 4
   load output dword from vsrc:offset
  else if size = 8
   load output qword from vsrc:offset
  end if
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

macro ASSEMBLE dest*, vsrc*, size*, offset*
 {
  local chunk
  if size = 8
    load chunk qword from vsrc:offset
    dq chunk
  else if size = 4
    load chunk dword from vsrc:offset
    dd chunk
  else if size = 2
    load chunk word from vsrc:offset
    dw chunk
  else if size = 1
    load chunk byte from vsrc:offset
    db chunk
  end if
 }

end if ;; _VTEXT_INC_

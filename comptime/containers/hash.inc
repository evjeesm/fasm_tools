if ~ definite _HASH_INC_
_HASH_INC_ equ 1

;;
;; Collection of useful hash functions
;;
macro HASH_VTEXT_SIMPLE vtext*, length*, output*
 {
  local hash, i, bitlen
  bitlen = 64
  hash = 0
  i = 0
  while i < length
    local char,shl_mask
    load char byte from vtext:i
    shl_mask = (1 shl (bitlen - 5)) - 1
    hash = qword ((hash and shl_mask) shl 5) xor (hash shr 27) xor char
    i = i + 1
  end while
  output = hash
 }

macro HASH_VALUE_SIMPLE value*, length*, output*
 {
  local hash, i, bitlen
  bitlen = 64
  hash = 0
  i = 0
  while i < length
    local char, shl_mask
    char = (value shr (8 * i)) and 0xFF
    shl_mask = (1 shl (bitlen - 5)) - 1
    hash = qword ((hash and shl_mask) shl 5) xor (hash shr 27) xor char
    i = i + 1
  end while
  output = hash
 }
end if ;; _HASH_INC_

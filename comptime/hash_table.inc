if ~ definite _HASH_TABLE_INC_
_HASH_TABLE_INC_ equ 1

include 'comptime.inc'
include 'hash.inc'
include 'vtext.inc'

macro HASH_TABLE tbl*, value_size*, capacity:10
 {
  VTEXT_PREALLOC tbl#.tmp, value_size
  tbl#.tbl.value_size = value_size
  tbl#.tbl.capacity = capacity
  tbl#.tbl.size = 0
  VTEXT_PREALLOC tbl, value_size * capacity
  tbl#.len = value_size * capacity
 }

macro HASH_TABLE_PUT tbl*, value*
 {
  local value_hash, offset, start_index, index, equal, is_zero, pad
  _CONV_VALUE_TO_VTEXT tbl#.tmp, value, tbl#.tbl.value_size
  HASH_TABLE_PUT_VTEXT tbl, tbl#.tmp
 }

macro HASH_TABLE_PUT_VTEXT tbl*, vtext*
 {
  local value_hash, offset, start_index, index, equal, is_zero, pad
  STATIC_ASSERT vtext#.len = tbl#.tbl.value_size,\
    "vtext size should match element size of the table!"
  HASH_VTEXT_SIMPLE vtext, tbl#.tbl.value_size, value_hash
  start_index = value_hash mod tbl#.tbl.capacity
  index = start_index
  while 1
    offset = index * tbl#.tbl.value_size
    VTEXT_CMP vtext, tbl, offset, tbl#.tbl.value_size, equal
    if ~ equal
      VTEXT_RANGE_CHAR_EQ tbl, offset, tbl#.tbl.value_size, 0x0, is_zero
      if is_zero
        VTEXT_WRITE tbl, offset, vtext
        break
      end if
      index = (index + 1) mod tbl#.tbl.capacity
      if index = start_index
        STATIC_ASSERT 0, "No space left in the table"
      end if
    else
      break
    end if
  end while
 }

macro HASH_TABLE_HAS_VALUE tbl*, value*, output*
 {
  local value_hash, offset, start_index, index, equal, pad
  _CONV_VALUE_TO_VTEXT tbl#.tmp, value, tbl#.tbl.value_size
  HASH_TABLE_HAS_VTEXT tbl, tbl#.tmp, output
 }

macro HASH_TABLE_HAS_VTEXT tbl*, vtext*, output*
 {
  local value_hash, offset, start_index, index, equal, pad
  STATIC_ASSERT vtext#.len = tbl#.tbl.value_size,\
    "vtext size should match element size of the table!"
  HASH_VTEXT_SIMPLE vtext, tbl#.tbl.value_size, value_hash
  start_index = value_hash mod tbl#.tbl.capacity
  index = start_index
  while 1
    offset = index * tbl#.tbl.value_size
    VTEXT_CMP vtext, tbl, offset, tbl#.tbl.value_size, equal
    if ~ equal
      index = (index + 1) mod tbl#.tbl.capacity
      if index = start_index
        output = 0
        break
      end if
    else
      output = 1
      break
    end if
  end while
 }

macro HASH_TABLE_REMOVE tbl*, value*
 {
  local value_hash, offset, start_index, index, equal, is_zero, pad
  _CONV_VALUE_TO_VTEXT tbl#.tmp, value, tbl#.tbl.value_size
  HASH_TABLE_REMOVE_VTEXT tbl, tbl#.tmp
 }

macro HASH_TABLE_REMOVE_VTEXT tbl*, vtext*
 {
  local value_hash, offset, start_index, index, equal, is_zero, pad
  STATIC_ASSERT vtext#.len = tbl#.tbl.value_size,\
    "vtext size should match element size of the table!"
  HASH_VTEXT_SIMPLE vtext, tbl#.tbl.value_size, value_hash
  start_index = value_hash mod tbl#.tbl.capacity
  index = start_index
  while 1
    offset = index * tbl#.tbl.value_size
    VTEXT_CMP vtext, tbl, offset, tbl#.tbl.value_size, equal
    if ~ equal
      index = (index + 1) mod tbl#.tbl.capacity
      if index = start_index
        break ; no stored value
      end if
    else
      VTEXT_FILL tbl, offset, tbl#.tbl.value_size, 0
      break
    end if
  end while
 }

macro HASH_TABLE_RESET tbl*
 {
   VTEXT_FILL tbl, 0, tbl#.tbl.capacity * tbl#.tbl.value_size, 0x0
 }

macro _CONV_VALUE_TO_VTEXT vdest*, value*, value_size*
 {
  VTEXT_SET vdest, value
  pad = value_size - vdest#.len
  if pad >= 0
    VTEXT_ASSEMBLE vdest, times pad db 0
  else ;; pad is negative
    STATIC_ASSERT pad >= 0, "arg '", value, "' has larger size that tbl value"
  end if
 }

end if ;; _HASH_TABLE_INC_

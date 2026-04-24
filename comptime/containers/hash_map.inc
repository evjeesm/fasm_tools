include '../comptime.inc'
include '../vtext.inc'
include 'hash.inc'

purge HASH_MAP
macro HASH_MAP map*, key_size*, value_size*, capacity:10
 {
  VTEXT_PREALLOC map#.map.key_tmp, key_size
  VTEXT_PREALLOC map#.map.value_tmp, value_size
  map#.map.key_size = key_size
  map#.map.value_size = value_size
  map#.map.entry_size = key_size + value_size
  map#.map.capacity = capacity
  VTEXT_PREALLOC map, (map#.map.entry_size)*capacity
  map#.len = (map#.map.entry_size)*capacity
 }

purge HASH_MAP_PUT
macro HASH_MAP_PUT map*, [key*, value*]
 {
 forward
  VALUE_TO_VTEXT map#.map.key_tmp, key, map#.map.key_size
  VALUE_TO_VTEXT map#.map.value_tmp, value, map#.map.value_size
  HASH_MAP_PUT_VTEXT map, map#.map.key_tmp, map#.map.value_tmp
 }

purge HASH_MAP_PUT_VTEXT
macro HASH_MAP_PUT_VTEXT map*, [vtext_key*, vtext_value*]
 {
 forward
  local key_hash, offset, start_index, index, is_zero
  STATIC_ASSERT vtext_key#.len = map#.map.key_size,\
    "vtext_key size should match element map specs"
  STATIC_ASSERT vtext_value#.len = map#.map.value_size,\
    "vtext_value size should match element map specs"
  HASH_VTEXT_SIMPLE vtext_key, map#.map.key_size, key_hash
  start_index = (key_hash mod map#.map.capacity)
  index = start_index
  while 1
    offset = index * map#.map.entry_size
    VTEXT_RANGE_CHAR_EQ map, offset, map#.map.entry_size, 0x0, is_zero
    if is_zero
      VTEXT_WRITE map, offset, vtext_key
      VTEXT_WRITE map, offset+map#.map.key_size, vtext_value
      break
    else
      index = (index + 1) mod map#.map.capacity
      if index = start_index
        STATIC_ASSERT 0, "No space left in hash map"
      end if
    end if
  end while
 }

purge HASH_MAP_GET
macro HASH_MAP_GET map*, key*, status*, output*
 {
  VALUE_TO_VTEXT map#.map.key_tmp, key, map#.map.key_size
  HASH_MAP_GET_VTEXT map, map#.map.key_tmp, map#.map.value_tmp
  if map#.map.value_tmp.len > 0
    status = 1
    ;;VTEXT_READ_FIELD map#.map.value_tmp, 0, map#.map.value_size, output
    VTEXT_READ_TO_VALUE map#.map.value_tmp, 0, map#.map.value_size, output
  else
    status = 0
  end if
 }

purge HASH_MAP_GET_VTEXT
macro HASH_MAP_GET_VTEXT map*, vtext_key*, voutput*
 {
  local key_hash, offset, start_index, index, is_zero, equal
  HASH_VTEXT_SIMPLE vtext_key, map#.map.key_size, key_hash
  start_index = (key_hash mod map#.map.capacity)
  index = start_index
  while 1
    offset = index * map#.map.entry_size
    VTEXT_RANGE_CHAR_EQ map, offset, map#.map.entry_size, 0x0, is_zero
    if is_zero
      voutput#.len = 0 ;; no result
      break
    else
      VTEXT_CMP vtext_key, map, offset, map#.map.key_size, equal
      if ~ equal
        index = (index + 1) mod map#.map.capacity
        if index = start_index
          voutput#.len = 0 ;; no result
          break
        end if
      else ;; match
        VTEXT_READ voutput, map, offset+map#.map.key_size, map#.map.value_size
        break
      end if
    end if
  end while
 }

purge HASH_MAP_REMOVE
macro HASH_MAP_REMOVE map*, key*
 {
  VALUE_TO_VTEXT map#.map.key_tmp, key, map#.map.key_size
  HASH_MAP_REMOVE_VTEXT map, map#.map.key_tmp
 }

purge HASH_MAP_REMOVE_VTEXT
macro HASH_MAP_REMOVE_VTEXT map*, vtext_key*
 {
  local key_hash, offset, start_index, index, is_zero, equal
  HASH_VTEXT_SIMPLE vtext_key, map#.map.key_size, key_hash
  start_index = (key_hash mod map#.map.capacity)
  index = start_index
  while 1
    offset = index * map#.map.entry_size
    VTEXT_RANGE_CHAR_EQ map, offset, map#.map.entry_size, 0x0, is_zero
    if is_zero
      break
    else
      VTEXT_CMP vtext_key, map, offset, map#.map.key_size, equal
      if ~ equal
        index = (index + 1) mod map#.map.capacity
        if index = start_index
          break
        end if
      else ;; match
        VTEXT_FILL map, offset, map#.map.entry_size, 0
        break
      end if
    end if
  end while
 }

purge HASH_MAP_RESET
macro HASH_MAP_RESET map*
 {
  VTEXT_FILL map, 0, map#.map.capacity * map#.map.entry_size, 0x0
 }

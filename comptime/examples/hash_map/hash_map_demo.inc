include '../../fmt.inc'
include '../../containers/hash_map.inc'

;; Test with value
HASH_MAP ..map, 1, 8
HASH_MAP_PUT ..map, 'a', 'hell_ove'

HASH_MAP_GET ..map, 'a', status, val
FMT %S val %NL

HASH_MAP_PUT ..map, 'b', 'word'
HASH_MAP_GET ..map, 'b', status, val
FMT %S val %NL

HASH_MAP_REMOVE ..map, 'a'

HASH_MAP_GET ..map, 'a', status, val
if status
  FMT %S val %NL
else
  FMT "NONE" %NL
end if

VTEXT ..key, 'c'
VTEXT ..value, '0101'
VTEXT_PAD_WITH ..value, 8, 0

HASH_MAP_PUT_VTEXT ..map, ..key, ..value

;; Reserve value for result
VTEXT_PREALLOC value, 8

;; Use predefined key to retrieve the value
HASH_MAP_GET_VTEXT ..map, ..key, value

;; Check that returned value is valid
if value.len
  VTEXT_DISPLAY value
else
  FMT "NONE" %NL
end if

VTEXT_SET ..value, '__8888__'
VTEXT ..^key, 'd'
VTEXT ..^value, '1010'
VTEXT_PAD_WITH ..^value, 8, 0

;; bulk put
HASH_MAP_PUT_VTEXT ..map, ..key, ..value, \
                          ..^key, ..^value

;; Use predefined key to retrieve the value
HASH_MAP_GET_VTEXT ..map, ..key, value

;; Check that returned value is valid
if value.len
  VTEXT_DISPLAY value
else
  FMT "NONE" %NL
end if

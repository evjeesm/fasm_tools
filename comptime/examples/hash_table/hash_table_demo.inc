
include '../../fmt.inc'
include '../../hash_table.inc'

HASH_TABLE ..t, 5, 10
HASH_TABLE_PUT ..t, 'world'
HASH_TABLE_PUT ..t, 'mfka1'
HASH_TABLE_PUT ..t, 'hel'
;; HASH_TABLE_RESET ..t

HASH_TABLE_REMOVE ..t, 'hel'

HASH_TABLE_HAS_VALUE ..t, 'mfka1', val
FMT "has val? " %D val %NL

HASH_TABLE_HAS_VALUE ..t, 'hel', val
FMT "has val? " %D val %NL

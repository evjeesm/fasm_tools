.PHONY: clean test

export INCLUDE=../..;../../../os/

test: embed_exec
	./embed_exec

embed_exec: embed_exec.asm
	fasm -m 2000000 -dPAYLOAD_CALL=$(PAYLOAD_CALL) embed_exec.asm
	chmod u+x $@

clean:
	rm embed_exec payload

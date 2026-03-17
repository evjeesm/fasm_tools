.PHONY: clean test

export INCLUDE=../..;../../../os/

test: embed_exec
	./embed_exec

embed_exec: embed_exec.asm payload
	fasm -m 2000000 -dPAYLOAD_CALL=$(PAYLOAD_CALL) embed_exec.asm
	chmod u+x $@

payload: payload.asm
	fasm -dPAYLOAD_CALL=$(PAYLOAD_CALL) payload.asm
	ld payload.o -o payload

clean:
	rm embed_exec payload

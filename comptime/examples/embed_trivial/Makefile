.PHONY: clean test

export INCLUDE=../..

test: embed_exec
	./embed_exec

embed_exec: payload
	fasm embed_exec.asm
	chmod u+x $@

payload:
	fasm payload.asm

clean:
	rm embed_exec payload

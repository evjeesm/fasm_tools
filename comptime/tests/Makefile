TEST_SOURCES := $(wildcard *.asm)
TEST_TARGETS := $(TEST_SOURCES:%.asm=%)

export INCLUDE := ..;../../os/

SHELL := /bin/bash

# Run test file and store output in a *.out file
# so then it can be used as a reference.
# [IMPORTANT] Manually confirm that expected output is correct!
gen-%: %.asm
	fasm $< /dev/null > $(<:.asm=.out)

# Test against pregenerated *.out file
%: %.asm
	diff --color <(fasm $< /dev/null) $(<:.asm=.out)

all: $(TEST_TARGETS)

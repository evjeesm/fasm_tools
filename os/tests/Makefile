TEST_SOURCES := $(wildcard *.asm)
TEST_TARGETS := $(TEST_SOURCES:%.asm=%)

export INCLUDE := ..;../../;../../comptime/
SHELL := /bin/bash
.PHONY: all tests clean

# Run test file and store output in a *.out file
# so then it can be used as a reference.
# [IMPORTANT] Manually confirm that expected output is correct!
gen-%: %
	./$< a b c > $(<:=.out)

%.o: %.asm
	fasm $<

%: %.o
	gcc -nostdlib -o $@ $<

test-%: %
	diff --color <(./$< a b c) $(<:%=%.out)

all: $(TEST_TARGETS)

tests: $(TEST_TARGETS:%=test-%)

clean:
	rm $(TEST_TARGETS)

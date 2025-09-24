TEST_SOURCES := $(wildcard *.asm)
TEST_TARGETS := $(TEST_SOURCES:%.asm=%)

export INCLUDE := ..

%: %.asm
	fasm $< /dev/null

all: $(TEST_TARGETS)


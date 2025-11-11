############################################################
# General purpose makefile
#
# Works for all simple C-projects where
# - binaries are compiled into sub-dir bin
# - binaries are created from a single c-source of the same name
#
# Note: multiple targets (binaries) in ./bin are supported
#

# Please add all targets in ./bin here

TARGETS += $(BIN_DIR)/incV1
TARGETS += $(BIN_DIR)/incV2

DISASS += incV1.c.disass
DISASS += incV2.c.disass

#################################################
# There is no need to edit below this line
#################################################

# Generate debugging symbols?
#CFLAGS = -O2 -g -Wall
CFLAGS = -g -Wall

#### Fixed variable definitions
CC = gcc
RM_DIR = rm -rf
MKDIR = mkdir
SHELL = /bin/bash
BIN_DIR = bin

####

all: $(BIN_DIR) $(TARGETS) $(DISASS)

#### Fixed build rules
$(BIN_DIR)/% : %.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

%.c.disass : $(BIN_DIR)/%
	gdb -q -ex "disassemble main" -ex "quit" $< > $@

.PHONY: clean
clean :
	$(RM_DIR) $(BIN_DIR) $(DISASS)


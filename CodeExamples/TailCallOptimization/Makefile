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

TARGETS += $(BIN_DIR)/fac
TARGETS += $(BIN_DIR)/itfac1
TARGETS += $(BIN_DIR)/itfac2
TARGETS += $(BIN_DIR)/itfac3

#################################################
# There is no need to edit below this line
#################################################

# Generate debugging symbols?
# Note: use commandline
#   make CFLAGS_ADD="-O2"
# to add the compiler flag -O2

CFLAGS = -g -Wall
CFLAGS += $(CFLAGS_ADD)

#LDFLAGS = -lrt
LDFLAGS =

# default for -mpreferred-stack-boundary=4 on 32/64bit machines
MACHINE := $(shell uname -m)
$(info $$MACHINE is $(MACHINE))
ifeq ($(MACHINE), i686)
  #EXTRA_FLAGS = -mpush-args -mpreferred-stack-boundary=4 -mno-accumulate-outgoing-args
  #EXTRA_FLAGS = -mpush-args -mpreferred-stack-boundary=2
  EXTRA_FLAGS =
else ifeq ($(MACHINE), armv7l)
  EXTRA_FLAGS =
else ifeq ($(MACHINE), x86_64)
  EXTRA_FLAGS =
endif

#### Fixed variable definitions
CC = gcc
RM_DIR = rm -rf
MKDIR = mkdir
SHELL = /bin/bash
BIN_DIR = bin

####

all: $(BIN_DIR) $(TARGETS) disass

#### Fixed build rules
$(BIN_DIR)/% : %.c
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

.PHONY: clean
clean :
	$(RM_DIR) $(BIN_DIR) *.disass

.PHONY: disass
disass :
	gdb -q bin/fac    -ex "disassemble fac"   -ex "quit" > fac.disass
	gdb -q bin/itfac1 -ex "disassemble itfac" -ex "quit" > itfac1.disass
	gdb -q bin/itfac2 -ex "disassemble itfac" -ex "quit" > itfac2.disass
	gdb -q bin/itfac3 -ex "disassemble itfac" -ex "quit" > itfac3.disass


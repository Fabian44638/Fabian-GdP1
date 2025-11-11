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

TARGETS += $(BIN_DIR)/euklid

#################################################
# There is no need to edit below this line
#################################################

# Generate debugging symbols?
CFLAGS = -g -Wall

# Query for build system
$(eval BSYSTEM = $(shell gcc -dumpmachine))
$(info Building on $(BSYSTEM))

# Set option for pic
ifeq ($(BSYSTEM),i686-linux-gnu)
    #EXTRA_FLAGS = -fno-pic -mpush-args -mno-accumulate-outgoing-args -mpreferred-stack-boundary=4
    EXTRA_FLAGS = -fno-pic
else
    EXTRA_FLAGS = -fpic
endif

#### Fixed variable definitions
CC = gcc
RM_DIR = rm -rf
MKDIR = mkdir
SHELL = /bin/bash
BIN_DIR = bin

####

all: $(BIN_DIR) $(TARGETS)

#### Fixed build rules
$(BIN_DIR)/% : %.c
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $< -o $@

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

clean :
	$(RM_DIR) $(BIN_DIR)

.PHONY : clean

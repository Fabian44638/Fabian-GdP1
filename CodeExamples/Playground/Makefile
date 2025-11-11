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

TARGETS += $(BIN_DIR)/helloArrayOutOfBound
TARGETS += $(BIN_DIR)/helloFakedReturn
TARGETS += $(BIN_DIR)/check_ASLR

#################################################
# There is no need to edit below this line
#################################################

# Generate debugging symbols?
CFLAGS = -g -Wall

MACHINE := $(shell uname -m)
$(info $$MACHINE is $(MACHINE))
ifeq ($(MACHINE), i686)
  CFLAGS += -Wno-array-bounds -fno-stack-protector -fomit-frame-pointer -momit-leaf-frame-pointer
else ifeq ($(MACHINE), armv7l)
  CFLAGS += -Wno-array-bounds -fno-stack-protector -fomit-frame-pointer
else ifeq ($(MACHINE), arm64)
  CFLAGS += -Wno-array-bounds -fno-stack-protector -fomit-frame-pointer
else ifeq ($(MACHINE), x86_64)
  CFLAGS += -Wno-array-bounds -fno-stack-protector -fomit-frame-pointer
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
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

.PHONY: clean
clean :
	$(RM_DIR) $(BIN_DIR)


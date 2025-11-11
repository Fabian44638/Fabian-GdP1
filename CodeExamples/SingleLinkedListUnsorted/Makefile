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

TARGETS += $(BIN_DIR)/singleLinkedIntList_functional_tail_recursive_demo7
TARGETS += $(BIN_DIR)/singleLinkedIntList_functional_tail_recursive_demo8
TARGETS += $(BIN_DIR)/singleLinkedIntList_functional_iterative_demo
TARGETS += $(BIN_DIR)/singleLinkedIntList_functional_recursive_demo
TARGETS += $(BIN_DIR)/singleLinkedIntList_showRecursion

#################################################
# There is no need to edit below this line
#################################################

# Generate debugging symbols?
CFLAGS = -g -Wall

MACHINE := $(shell uname -m)
$(info $$MACHINE is $(MACHINE))
ifeq ($(MACHINE), i686)
  CFLAGS += -O2 -fno-inline-small-functions
else ifeq ($(MACHINE), x86_64)
  CFLAGS += -O2 -fno-inline-small-functions
else ifeq ($(MACHINE), armv7l)
  CFLAGS += -O2 -fno-inline-small-functions
else
  CFLAGS += -O2 
endif

#### Fixed variable definitions
CC = gcc
RM_DIR = rm -rf
MKDIR = mkdir
SHELL = /bin/bash
BIN_DIR = bin

####

all: $(BIN_DIR) $(TARGETS)

#### Special

#### Recipes for targets
$(BIN_DIR)/singleLinkedIntList_functional_iterative_demo: \
	dumpListForDot.o \
	singleLinkedIntList_functional_iterative.o \
	singleLinkedIntList_functional_demo.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/singleLinkedIntList_showRecursion: \
	dumpListForDot.o \
	singleLinkedIntList_functional_recursive.o \
	singleLinkedIntList_showRecursion.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/singleLinkedIntList_functional_recursive_demo: \
	dumpListForDot.o \
	singleLinkedIntList_functional_recursive.o \
	singleLinkedIntList_functional_demo.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/singleLinkedIntList_functional_tail_recursive_demo7: \
	singleLinkedIntList_functional_tail_recursive_insert_at_end.o \
	singleLinkedIntList_functional_tail_recursive_demo7.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/singleLinkedIntList_functional_tail_recursive_demo8: \
	singleLinkedIntList_functional_tail_recursive_remove_at_end.o \
	singleLinkedIntList_functional_tail_recursive_demo8.o
	$(CC) $(CFLAGS) $^ -o $@

#### Dependencies
dumpListForDot.o: dumpListForDot.h singleLinkedIntList_type.h
singleLinkedIntList_functional_recursive.o: singleLinkedIntList_functional.h singleLinkedIntList_type.h
singleLinkedIntList_functional_iterative.o: singleLinkedIntList_functional.h singleLinkedIntList_type.h
singleLinkedIntList_functional_demo.o: dumpListForDot.h singleLinkedIntList_functional.h singleLinkedIntList_type.h
singleLinkedIntList_functional_recursive_demo.o: dumpListForDot.h singleLinkedIntList_functional.h singleLinkedIntList_type.h

#### Fixed build rules
$(BIN_DIR)/% : %.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

.PHONY: clean
clean :
	$(RM_DIR) $(BIN_DIR) *.o *.dot


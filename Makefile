THIS_MAKE_FILE     := $(abspath $(lastword $(MAKEFILE_LIST)))
THIS_MAKE_FILE_DIR := $(realpath $(patsubst %/,%,$(dir $(THIS_MAKE_FILE))))


# EXECS
MKDIR   := mkdir -p
RM      := rm -rf
CC      := gcc
CFLAGS  := -g -O0 -Wall -Wextra -Wpedantic -Werror -std=c11 --sanitize=address,undefined
LDFLAGS :=

CFLAGS += -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 
CFLAGS += -Wconversion -Wsign-conversion -Wformat-security -Wnull-dereference
CFLAGS += -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4
CFLAGS += -pie -fPIE -ftrapv -D_FORTIFY_SOURCE=3
CFLAGS += -Wtrampolines -Walloca -Wvla -Warray-bounds=2 -Wimplicit-fallthrough=3 
CFLAGS += -Wshift-overflow=2 -Wcast-qual -Wstringop-overflow=4 
CFLAGS += -Warith-conversion -Wlogical-op -Wduplicated-cond -Wduplicated-branches -Wformat-signedness
CFLAGS += -Wshadow -Wstrict-overflow=4 -Wundef -Wstrict-prototypes -Wswitch-default -Wstack-usage=1000000 
CFLAGS += -Wcast-align=strict -fstack-protector-strong -fstack-clash-protection 
CFLAGS += -fsanitize=bounds -fsanitize-undefined-trap-on-error

# Stashed flags:
# CFLAGS += -Wtraditional-conversion -Wswitch-enum


# LDFLAGS += -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,-z,separate-code -Wl,dynamicbase -Wl,nxcompat


# DIRS
BUILD_DIR  := $(THIS_MAKE_FILE_DIR)/build
SRC_DIR    := $(THIS_MAKE_FILE_DIR)/src
OBJ_DIR    := $(BUILD_DIR)/objs

# BINS
EXEC       := $(BUILD_DIR)/rpn

# TESTS
TESTS_SRCS := $(SRC_DIR)/tests.c
TESTS_BIN  := $(BUILD_DIR)/tests

# SRCS
HEADERS  := $(wildcard $(SRC_DIR)/*.h)
SRCS     := $(wildcard $(SRC_DIR)/*.c)
SRCS     := $(filter-out $(TESTS_SRCS), $(SRCS))
OBJS     := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
LIB_OBJS := $(filter-out $(OBJ_DIR)/main.o, $(OBJS))

default: all

build: dirs $(EXEC)

all: release

release: CFLAGS=-Wall -Wextra -Werror -std=c11 -O3 -D __RELEASE__ -D __NO_LOGS__
release: clean build


release-tests: release
	echo "x*x" | $(EXEC)
	echo "x" | $(EXEC)
	echo "sin(x)" | $(EXEC)
	echo "x*x*x" | $(EXEC)
	echo "x+x*x" | $(EXEC)
	echo "1/x" | $(EXEC)
	echo "tan(x)" | $(EXEC)
	echo "x*sin(x)" | $(EXEC)
	echo "sin(sin(sin(sin(1/ln(x*x)))))" | $(EXEC)
	echo "1" | $(EXEC)
	echo "-1" | $(EXEC)
	echo "-x*2" | $(EXEC)
	echo "sqrt(-(1/(-x*x)))" | $(EXEC)
	echo "(x+1)*(2*x-4)" | $(EXEC)
	echo "(x*x-x*x*x)/4" | $(EXEC)

# Make dirs and add .gitignore to build/ folder (in order to ignore all content)
dirs:
	[[ ! -d $(BUILD_DIR) ]] && $(MKDIR) $(BUILD_DIR) || true
	[[ ! -f $(BUILD_DIR)/.gitignore ]] && touch $(BUILD_DIR)/.gitignore || true
	[[ -f $(BUILD_DIR)/.gitignore ]] && echo "*" > $(BUILD_DIR)/.gitignore || true
	[[ ! -d $(OBJ_DIR) ]] && $(MKDIR) $(OBJ_DIR) || true

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@ $(LDFLAGS)

$(TESTS_BIN): $(TESTS_SRCS) $(LIB_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test: dirs $(TESTS_BIN)
	leaks --atExit -- $(TESTS_BIN)

clean:
	$(RM) $(BUILD_DIR)/*

fmt: 
	clang-format -i $(HEADERS) $(SRCS)

lint:
	clang-format -n -Werror $(HEADERS) $(SRCS)
	cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_DIR)

run: all
	leaks --atExit -- $(EXEC)

hooks:
	cp $(SRC_DIR)/hooks/lint.sh $(THIS_MAKE_FILE_DIR)/.git/hooks/pre-commit

.PHONY: default build all release release-tests dirs test clean fmt lint run hooks
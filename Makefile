DEBUG ?= 0

CFLAGS = -Wall -Wextra -Werror -Wno-unused -pedantic -Ilib
LDFLAGS = 

ifeq ($(DEBUG),1)
    CFLAGS += -g -fsanitize=address
    LDFLAGS += -fsanitize=address
endif

SRC_DIR = src
LIB_SRC = $(wildcard lib/*.c)
LIB_OBJ = $(LIB_SRC:%.c=build/%.o)
LIB = build/lib/libcryptoutils.a
SOURCES = $(wildcard $(SRC_DIR)/*.c)
TARGETS = $(SOURCES:%.c=build/%)

ifeq ($(DEBUG),1)
    DSYM_TARGETS = $(TARGETS:=.dSYM)
else
    DSYM_TARGETS =
endif

all: $(TARGETS) $(DSYM_TARGETS)

build/%: build/%.o $(LIB)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $^ -o $@

build/%.dSYM: build/%
	dsymutil $< -o $@

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(LIB_OBJ)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

clean:
	rm -rf build

.PHONY: all clean

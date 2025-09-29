CFLAGS = -Wall -Wextra -Werror -Wno-unused -pedantic -g -Ilib
LDFLAGS = 

SRC_DIR = src
LIB_SRC = $(wildcard lib/*.c)
LIB_OBJ = $(LIB_SRC:%.c=build/%.o)
LIB = build/lib/libcryptoutils.a

SOURCES = $(wildcard $(SRC_DIR)/*.c)
TARGETS = $(SOURCES:%.c=build/%)

all: $(TARGETS)

build/%: build/%.o $(LIB)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $^ -o $@

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(LIB_OBJ)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build

.PHONY: all clean

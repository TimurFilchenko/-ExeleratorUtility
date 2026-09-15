CC       ?= x86_64-w64-mingw32-clang
CPPFLAGS = -Iinclude
CFLAGS   = -Wall -Wextra -Wpedantic -O2
LDFLAGS  =
LDLIBS   =

TARGET = build/Exelerator.exe

SRC = \
	src/main.c \
	src/core/app.c \
	src/scanner/scanner.c \
	src/scanner/walker.c \
	src/hash/hash.c \
	src/hash/sha256.c \
	src/duplicate/duplicate.c \
	src/duplicate/group.c \
	src/file/file.c \
	src/file/delete.c \
	src/ui/console.c

OBJ = $(SRC:%.c=build/%.o)
DEP = $(OBJ:.o=.d)

TEST_SCANNER   = build/test_scanner.exe
TEST_DUPLICATE = build/test_duplicate.exe

LIB_OBJ = $(filter-out build/src/main.o,$(OBJ))

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(TEST_SCANNER): tests/test_scanner.c $(LIB_OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(TEST_DUPLICATE): tests/test_duplicate.c $(LIB_OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

test: $(TEST_SCANNER) $(TEST_DUPLICATE)
	@status=0; \
	./$(TEST_SCANNER) || status=1; \
	./$(TEST_DUPLICATE) || status=1; \
	exit $$status

clean:
	rm -rf build

-include $(DEP)

CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -pedantic -I.
TARGET = cmatch
TEST_BIN = test_cmatch
LIB_SRCS = src/match.c src/table.c src/io.c
TEST_SRCS = tests/test_match.c

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(LIB_SRCS) main.c
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_BIN): $(LIB_SRCS) $(TEST_SRCS)
	$(CC) $(CFLAGS) $^ -o $@

test: $(TEST_BIN)
	./$(TEST_BIN) && rm -f $(TEST_BIN)

clean:
	rm -f $(TARGET) $(TEST_BIN)

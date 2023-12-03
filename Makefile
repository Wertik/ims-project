TARGET=path
TEST_TARGET=path_test

CC=gcc

SOURCES=$(shell find . -path '*.c' ! -path './test.c')
HEADERS=$(shell find . -path '*.h')

OBJECTS=$(subst .c,.o,$(SOURCES))

TEST_SOURCES=$(shell find . -path '*.c' ! -path './path.c')
TEST_OBJECTS=$(subst .c,.o,$(TEST_SOURCES))

CFLAGS=-g -Wall -Wextra -pedantic
LDFLAGS=-lSDL2

.PHONY: all

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(OBJECTS) $(HEADERS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJECTS) $(HEADERS)
	$(CC) $(CFLAGS) $(TEST_OBJECTS) -o $(TEST_TARGET) $(LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	-rm -rf $(TARGET) $(TEST_TARGET) $(OBJECTS) $(TEST_OBJECTS) sdl.o


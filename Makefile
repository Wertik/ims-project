TARGET=path
TEST_TARGET=path_test

CC=gcc

SOURCES=$(shell find . -path '*.c' ! -path './test.c')
HEADERS=$(shell find . -path '*.h')

OBJECTS=$(subst .c,.o,$(SOURCES))

TEST_SOURCES=$(shell find . -path '*.c' ! -path './path.c')
TEST_OBJECTS=$(subst .c,.o,$(TEST_SOURCES))

CFLAGS=-g -Wall -Wextra -pedantic
LDFLAGS=-lSDL2 -lm

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

simulation: $(TARGET)
	rm -f results.txt
	for i in 10 30 50 80 100 200 400 600 1000 1500; do \
		echo "Simulace count=$$i:" >> results.txt; \
		./$(TARGET) -l -m 3 -s 1 -c $$i >> results.txt; \
	done

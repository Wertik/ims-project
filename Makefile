TARGET=path

CC=gcc

SOURCES=$(shell find . -path '*.c')
HEADERS=$(shell find . -path '*.h')
OBJECTS=$(subst .c,.o,$(SOURCES))

CFLAGS=-Wall -Wextra -pedantic
LDFLAGS=-lSDL2

$(TARGET): $(OBJECTS) $(HEADERS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	-rm -rf $(TARGET) $(OBJECTS)


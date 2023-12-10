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

MAP_NO_RIGHTWAY_RESULTS_FILE=no_rightway.csv
MAP_RIGHTWAY_RESULTS_FILE=rightway.csv

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
	-rm -rf $(TARGET) $(TEST_TARGET) $(OBJECTS) $(TEST_OBJECTS) $(RESULTS_FILE) sdl.o

simulation: $(TARGET)
	rm -f $(MAP_NO_RIGHTWAY_RESULTS_FILE)
	echo "simulation_count,avg_until_parked,avg_until_leave,avg_inter_wait,perc_left_without_park" >> $(MAP_NO_RIGHTWAY_RESULTS_FILE) 
	for i in 10 30 50 100 200 400 600 1000 1500 3000; do \
		echo "Running for $$i cars"; \
		echo -n "$$i," >> $(MAP_RIGHTWAY_RESULTS_FILE); \
		./$(TARGET) -l -m 3 -s 0 -c $$i -v >> $(MAP_NO_RIGHTWAY_RESULTS_FILE); \
	done

	rm -f $(MAP_RIGHTWAY_RESULTS_FILE)
	echo "simulation_count,avg_until_parked,avg_until_leave,avg_inter_wait,perc_left_without_park" >> $(MAP_RIGHTWAY_RESULTS_FILE) 
	for i in 10 30 50 100 200 400 600 1000 1500 3000; do \
		echo "Running for $$i cars"; \
		echo -n "$$i," >> $(MAP_NO_RIGHTWAY_RESULTS_FILE); \
		./$(TARGET) -l -m 4 -s 0 -c $$i -v >> $(MAP_RIGHTWAY_RESULTS_FILE); \
	done

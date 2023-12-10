TARGET=path
TEST_TARGET=path_test

CC=gcc

HEADERS=$(shell find . -path '*.h')

PRE_SOURCES=$(shell find . -path '*.c' ! -path './test.c')
TEST_PRE_SOURCES=$(shell find . -path '*.c' ! -path './path.c')

ifdef GRAPH
  CFLAGS=-g -Wall -Wextra -pedantic -DGRAPH
  LDFLAGS=-lSDL2 -lm
  SOURCES=$(PRE_SOURCES)
  TEST_SOURCES=$(TEST_PRE_SOURCES)
else
CFLAGS=-g -Wall -Wextra -pedantic
  LDFLAGS=-lm
  SOURCES=$(filter-out ./graph.c,$(PRE_SOURCES))
  TEST_SOURCES=$(filter-out ./graph.c,$(TEST_PRE_SOURCES))
endif

OBJECTS=$(subst .c,.o,$(SOURCES))
TEST_OBJECTS=$(subst .c,.o,$(TEST_SOURCES))

MAP_NO_RIGHTWAY_RESULTS_FILE=no_rightway.csv
MAP_RIGHTWAY_RESULTS_FILE=rightway.csv

ARCHIVE_NAME=T8_xkucaj01_xotrad00

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
	-rm -rf $(TARGET) $(TEST_TARGET) $(OBJECTS) $(TEST_OBJECTS) $(RESULTS_FILE) $(ARCHIVE_NAME).tar.gz

pack:
	tar cvzf $(ARCHIVE_NAME).tar.gz *.c *.h Makefile

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

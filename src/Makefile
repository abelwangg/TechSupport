CC      = gcc
CFLAGS  = -Wall -Wextra -g -std=c99 -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lncurses

SOURCES    = main.c ds.c game.c persist.c utils.c visualize.c
OBJECTS    = $(SOURCES:.c=.o)
EXECUTABLE = techsupport

TEST_SOURCES    = tests.c ds.c persist.c utils.c test_globals.c
TEST_OBJECTS    = $(TEST_SOURCES:.c=.o)
TEST_EXECUTABLE = run_tests

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c lab4.h
	$(CC) $(CFLAGS) -c $< -o $@

tests: $(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(EXECUTABLE) $(TEST_EXECUTABLE)
	rm -f techsupport.dat test_ts.dat test_ts2.dat *.o

run: $(EXECUTABLE)
	./$(EXECUTABLE)

test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

valgrind: $(EXECUTABLE)
	valgrind --leak-check=full --show-leak-kinds=all \
	         --track-origins=yes ./$(EXECUTABLE)

valgrind-test: $(TEST_EXECUTABLE)
	valgrind --leak-check=full --show-leak-kinds=all \
	         --track-origins=yes ./$(TEST_EXECUTABLE)

help:
	@echo "Targets: all  tests  run  test  clean  valgrind  valgrind-test"

.PHONY: all tests clean run test valgrind valgrind-test help

CC = gcc
CFLAGS = --pedantic -Wall -Wextra -Wmissing-prototypes -std=c99
TARGET = cpuScheduler
EXAMPLE_TARGET = example

SRCS = computer.c graph.c main.c schedulingAlgorithms.c schedulingLogic.c simulation.c stats.c utils.c queues.c
HEADERS = computer.h graph.h schedulingAlgorithms.h schedulingLogic.h simulation.h stats.h utils.h process.h queues.h
EXAMPLE_SRCS = graph_stats_example.c graph.c stats.c

OBJS = $(SRCS:.c=.o)
EXAMPLE_OBJS = $(EXAMPLE_SRCS:.c=.o)

.PHONY: all build_example clean

all: $(TARGET)

build_example: $(EXAMPLE_TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

$(EXAMPLE_TARGET): $(EXAMPLE_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET) $(EXAMPLE_OBJS) $(EXAMPLE_TARGET)


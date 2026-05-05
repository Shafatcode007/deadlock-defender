CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude
LIBS    = -lpthread
TARGET  = deadlock

SRCS    = src/main.c \
          src/ipc.c  \
          src/banker.c \
          src/worker.c \
          src/manager.c

OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean

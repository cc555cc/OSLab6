CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = group8_manager
SRC = group8_manager.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

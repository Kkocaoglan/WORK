# Bubble Shooter Makefile
# Raylib ile derleme (Linux/macOS/Windows)

CC = gcc
CFLAGS = -Wall -std=c99
SRCS = main.c game.c bubble.c player.c collision.c ui.c utils.c
OBJS = $(SRCS:.c=.o)
TARGET = bubbleshooter

# Raylib linkleme (Linux/macOS)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LIBS = -lraylib -lm
endif
ifeq ($(UNAME_S),Darwin)
    LIBS = -lraylib -lm
endif
# Windows için (MinGW)
ifeq ($(OS),Windows_NT)
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lm
    TARGET := bubbleshooter.exe
endif

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET) *.o 
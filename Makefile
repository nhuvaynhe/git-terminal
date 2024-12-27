# Makefile for building the program with ncurses or PDCurses

# Compiler and flags
CC = gcc
CFLAGS = -DNCURSES_STATIC
LDFLAGS = -lncurses

# Output executable
TARGET = main

# Source files
SRCS = main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Detect the platform
ifeq ($(OS),Windows_NT)
    RM = del /f /q  # Use Windows delete command
    EXE = .exe      # Executable extension for Windows
else
    RM = rm -f      # Use Unix/Linux delete command
    EXE =           # No extension for Unix/Linux executables
endif


# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile .c files to .o files
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Clean up build files
clean:
	$(RM) $(OBJS) $(TARGET)$(EXE)

# Rebuild everything
rebuild: clean all

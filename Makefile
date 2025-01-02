# Makefile for building the program with ncurses or PDCurses

# Compiler and flags
CC = gcc
CFLAGS = -DNCURSES_STATIC
LDFLAGS = -lncurses

# Output executable
TARGET = main

# Source files
SRCS = main.c branches/branches.c keyprocess/keyproc.c process/proc.c

# Object files
OBJS = main.o branches\branches.o keyprocess\keyproc.o process\proc.o

# Detect the platform
ifeq ($(OS),Windows_NT)
    RM = del /f /q
    EXE = .exe
else
    RM = rm -f
    EXE =
endif

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET)$(EXE) $(OBJS) $(LDFLAGS)

# Compile .c files to .o files
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Clean up build files
clean:
ifeq ($(OS),Windows_NT)
	$(RM) main.o branches\\branches.o keyprocess\\keyproc.o process\\proc.o $(TARGET)$(EXE)
else
	$(RM) main.o branches/branches.o keyprocess/keyproc.o process/proc.o $(TARGET)
endif

# Rebuild everything
rebuild: clean all

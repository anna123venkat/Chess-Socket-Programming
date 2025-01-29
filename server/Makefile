# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Output binary
TARGET = server

# Source files
SRCS = server.c board.c

# Object files
OBJS = $(SRCS:.c=.o)

# Build the server executable
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the server
run: $(TARGET)
	./$(TARGET)


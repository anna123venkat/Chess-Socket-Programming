# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Output binary
TARGET = client

# Source files
SRCS = client.c board.c

# Object files
OBJS = $(SRCS:.c=.o)

# Build the client executable
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the client
run: $(TARGET)
	./$(TARGET) <server_ip> <port>


CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lgdbm

# Sources and directories
SRC_DIR = src
HEAD_DIR = include

COMMON_SOURCES = $(SRC_DIR)/trip_functions.c \
				 $(SRC_DIR)/reservation_functions.c

MANAGER_SRC = $(SRC_DIR)/bus_manager.c
CLIENT_SRC = $(SRC_DIR)/bus_client.c
SERVER_SRC = $(SRC_DIR)/bus_server.c

# Objects
COMMON_OBJ = $(COMMON_SOURCES:.c=.o)
MANAGER_OBJ = $(MANAGER_SRC:.c=.o) $(COMMON_OBJ)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o) $(COMMON_OBJ)
SERVER_OBJ = $(SERVER_SRC:.c=.o) $(COMMON_OBJ)

# Target
TARGET = bus_manager bus_client bus_server
all: $(TARGET)


bus_manager: $(MANAGER_OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

bus_client: $(CLIENT_OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

bus_server: $(SERVER_OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(HEAD_DIR)/bus.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)

run_manager: bus_manager
	./bus_manager

run_client: bus_client
	./bus_client

run_server: bus_server
	./bus_server

.PHONY: all clean run_manager run_client run_server
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 64

pthread_mutex_t general_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t player_to_join = PTHREAD_COND_INITIALIZER;

// Chessboard representation
wchar_t **create_board();
void free_board(wchar_t **board);
void initialize_board(wchar_t **board);

// Utility Functions
bool is_syntax_valid(const char *move);
bool is_move_valid(wchar_t **board, char *move);
bool is_diagonal_clear(wchar_t **board, char *start, char *end);
bool is_rect_clear(wchar_t **board, char *start, char *end);

// Communication Functions
void emit(int socket, const char *message);

// Game Room Thread
void *game_room(void *args);

// Main Function
int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept");
            continue;
        }

        printf("New connection accepted\n");

        pthread_t thread_id;
        int *socket_ptr = malloc(sizeof(int));
        if (!socket_ptr) {
            perror("malloc failed");
            close(new_socket);
            continue;
        }

        *socket_ptr = new_socket;

        if (pthread_create(&thread_id, NULL, game_room, socket_ptr) != 0) {
            perror("Thread creation failed");
            free(socket_ptr);
            close(new_socket);
        }
    }

    return 0;
}

void *game_room(void *args) {
    int player_socket = *(int *)args;
    free(args);

    wchar_t **board = create_board();
    initialize_board(board);

    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(player_socket, buffer, BUFFER_SIZE);
        if (valread <= 0) {
            printf("Player disconnected\n");
            break;
        }

        printf("Received: %s\n", buffer);

        if (!is_syntax_valid(buffer)) {
            emit(player_socket, "Invalid move syntax\n");
            continue;
        }

        if (!is_move_valid(board, buffer)) {
            emit(player_socket, "Invalid move\n");
            continue;
        }

        emit(player_socket, "Move accepted\n");
        // Update board logic would go here
    }

    free_board(board);
    close(player_socket);
    return NULL;
}

wchar_t **create_board() {
    wchar_t **board = malloc(8 * sizeof(wchar_t *));
    if (!board) {
        perror("malloc failed for board");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 8; i++) {
        board[i] = malloc(8 * sizeof(wchar_t));
        if (!board[i]) {
            perror("malloc failed for board row");
            exit(EXIT_FAILURE);
        }
    }

    return board;
}

void free_board(wchar_t **board) {
    for (int i = 0; i < 8; i++) {
        free(board[i]);
    }
    free(board);
}

void initialize_board(wchar_t **board) {
    // Simplified board initialization for demonstration
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = L'-';
        }
    }
}

bool is_syntax_valid(const char *move) {
    // Placeholder for syntax validation logic
    return strlen(move) == 4;
}

bool is_move_valid(wchar_t **board, char *move) {
    // Placeholder for move validation logic
    return true;
}

void emit(int socket, const char *message) {
    send(socket, message, strlen(message), 0);
}

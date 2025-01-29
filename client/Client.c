#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include "board.c"

#define RED   "\x1B[31m"
#define RESET "\x1B[0m"
#define GREEN "\x1B[32m"

// Define a mutex for thread synchronization
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *on_signal(void *sockfd) {
    char buffer[64];
    int n;
    int socket = *(int *)sockfd;
    int *player = (int *)malloc(sizeof(int)); // Fix 1: Correct allocation

    while (1) {
        bzero(buffer, 64);
        n = read(socket, buffer, 64);

        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }

        pthread_mutex_lock(&mutex); // Lock access to shared resources

        if (buffer[0] == 'i' || buffer[0] == 'e' || buffer[0] == '\0') {
            if (buffer[0] == 'i') {
                if (buffer[2] == 't') {
                    printf("\nMake your move: \n");
                }
                if (buffer[2] == 'n') {
                    printf("\nWaiting for opponent...\n");
                }
                if (buffer[2] == 'p') {
                    *player = atoi(&buffer[3]);
                    if (*player == 2) {
                        printf("You're blacks (%c)\n", buffer[3]);
                    } else {
                        printf("You're whites (%c)\n", buffer[3]);
                    }
                }
            } else if (buffer[0] == 'e') {
                // Syntax errors
                if (buffer[2] == '0') {
                    switch (buffer[3]) {
                        case '0':
                            printf(RED "  ↑ ('-' missing)\n" RESET);
                            break;
                        case '1':
                            printf(RED "↑ (should be letter)\n" RESET);
                            break;
                        case '2':
                            printf(RED "   ↑ (should be letter)\n" RESET);
                            break;
                        case '3':
                            printf(RED " ↑ (should be number)\n" RESET);
                            break;
                        case '4':
                            printf(RED " ↑ (out of range)\n" RESET);
                            break;
                        case '5':
                            printf(RED "   ↑ (should be number)\n" RESET);
                            break;
                        case '6':
                            printf(RED "   ↑ (out of range)\n" RESET);
                            break;
                        case '7':
                            printf(RED "(out of range)\n" RESET);
                            break;
                    }
                }
                printf("\nerror %s\n", buffer);
            }
        } else {
            // Print the board
            system("clear");
            if (*player == 1) {
                print_board_buff(buffer);
            } else {
                print_board_buff_inverted(buffer);
            }
        }

        pthread_mutex_unlock(&mutex); // Unlock access to shared resources
        bzero(buffer, 64);
    }
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    setlocale(LC_ALL, "en_US.UTF-8");
    char buffer[64];

    if (argc < 2) {
        fprintf(stderr, "Usage: %s hostname [port]\n", argv[0]);
        exit(1);
    }

    if (argc == 3) {
        portno = atoi(argv[2]);
    } else {
        portno = 80;
    }

    printf("Connecting to %s:%d\n", argv[1], portno);

    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    pthread_t tid[1];

    // Response thread
    pthread_create(&tid[0], NULL, &on_signal, &sockfd);

    while (1) {
        bzero(buffer, 64);
        fgets(buffer, 64, stdin);

        /* Send message to the server */
        n = write(sockfd, buffer, strlen(buffer));

        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
    }

    return 0;
}

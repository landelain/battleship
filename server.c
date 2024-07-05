#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>


char* logo =
    "\n\nBATTLESHIPS\n\n"
    "                                   )___(\\\n"
    "                           _______/__/_\\\n"
    "                  ___     /===========|   ___\n"
    " ____       __   [\\\\\\]___/____________|__[///]   __\n"
    " \\   \\_____[\\\\]__/___________________________\\\\__[//]___\n"
    "  \\A+L                                                 |\n"
    "   \\                                                  /\n"
    "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n"
    "Game by Angela and Lucas\n\n";

//network
int port;
int connected = 0; 
struct sockaddr_in clients[2];

int is_new_client(struct sockaddr_in address);
int handle_new_client(struct sockaddr_in address);
int process_demand(int sockfd, char* msg, int client_n);
int start_game();

//logic
int size, carriers, battleships, destroyers, submarines; 


int main(int argc, char* argv[]){

    if(argc<7){
        fprintf(stderr, "Error : missing arguments");
        return 1;
    }

    port = atoi(argv[1]);

    size = atoi(argv[2]);
    carriers = atoi(argv[3]);
    battleships = atoi(argv[4]);
    destroyers = atoi(argv[5]);
    submarines = atoi(argv[6]);

    // socket creation 

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        fprintf(stderr,"Error: socket creation failed\n");
        return 1;
    }

    struct sockaddr_in *server_addr = malloc(sizeof(struct sockaddr_in));
    memset(server_addr, 0, sizeof(server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr->sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)server_addr, sizeof(*server_addr)) == -1) {
        fprintf(stderr,"Error: bind failed\n");
        close(sockfd);
        free(server_addr);
        return 1;
    }

    system("clear");
    printf("%s", logo);
    printf("\nThe server is on with parameters :\n-port:%d\n-size:%d\n-carriers:%d\n-battleships:%d\n-destroyers:%d\n-submarines:%d\n\n",port, size, carriers, battleships, destroyers, submarines);

    while(1){

        struct sockaddr_in *temp_client_addr = malloc(sizeof(struct sockaddr_in));
        socklen_t temp_client_addr_len = sizeof(*temp_client_addr);
        char msg[500];

        ssize_t received_len = recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr *)temp_client_addr, &temp_client_addr_len);
        if (received_len > 0) {
            msg[received_len] = '\0';
        }

        int client_n = is_new_client(*temp_client_addr);
        if(client_n == -1){
            client_n = handle_new_client(*temp_client_addr);
        }

        if(client_n != -1){
            int flag = process_demand(sockfd, msg, client_n);

        }
        free(temp_client_addr);
    }
    close(sockfd);
    free(server_addr);

    return 0;
}

// network

int is_new_client(struct sockaddr_in address){

    for(int i = 0; i<connected; i++){
        if(clients[i].sin_addr.s_addr == address.sin_addr.s_addr && clients[i].sin_port == address.sin_port){
            return i;
        }
    }
    
    return -1;
}

int handle_new_client(struct sockaddr_in address){

    if(connected < 2){

        clients[connected] = address;
        connected += 1;
        fprintf(stderr,"server: client %d connected\n", connected);
        return connected-1;
    }

    return -1;
}


/*
Not sure what we're trying to do here, might be useful to have the message type to be an integer so we can easily check:
msg[0] == 0 --> msg is of type TXT, so if *(msg+2) == "Hello" --> client is trying to connect

I'd have a function to split message type and info and store that in a struct, then have different functions to handle different
message types (check tic-tac-toe project)



IDK what you're trying to do here but strchr(msg, ' ') acts weirdly because there is no space in the received message: "hello"
so later on it causes a segmentation fault in strncpy

What are code, instruction and delimiter supposed to be?
*/
int process_demand(int sockfd, char* msg, int client_n){

    if(atoi(msg) == 0){ // initial message
        if (sendto(sockfd, logo, strlen(logo), 0, (const struct sockaddr*)&clients[client_n], sizeof(clients[client_n])) == -1){
            fprintf(stderr, "Error : sending message");
            close(sockfd);
            return 1;
        }
        if(connected ==2){
            start_game(sockfd);
        }
    }

    return 0;
}

// logic

int start_game(int sockfd){

    char msg[2][100];
    strcpy(msg[0], "server: the game is starting, you will play as player 1\n\n");
    strcpy(msg[1], "server: the game is starting, you will play as player 2\n\n");

    for(int i = 0; i<2; i++){
        if (sendto(sockfd, msg[i], strlen(msg[i]), 0, (const struct sockaddr*)&clients[i], sizeof(clients[i])) == -1){
            fprintf(stderr, "Error : sending message to player %d", i);
            close(sockfd);
            return 1;
        }  
    }

    fprintf(stderr, "server: the game is starting\n\n");
    
    return 0;
}
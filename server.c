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
"BATTLESHIPS\n\n"
"                                   )___(\n"
"                           _______/__/_\n"
"                  ___     /===========|   ___\n"
" ____       __   [\\\\\\]___/____________|__[///]   __\n"
" \\   \\_____[\\\\]__/___________________________\\\\__[//]___\n"
"  \\A+L                                                 |\n"
"   \\                                                  /\n"
"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n"
"Game by Angela and Lucas\n";

//network
int port;
int connected = 0; 
struct sockaddr_in clients[2];

int is_new_client(struct sockaddr_in address);
int handle_new_client(struct sockaddr_in address);
int process_demand(int sockfd, char* msg, int client_n);

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

    printf("\nThe server is listening\n\n");

    while(1){

        struct sockaddr_in *temp_client_addr = malloc(sizeof(struct sockaddr_in));
        socklen_t temp_client_addr_len = sizeof(*temp_client_addr);
        char msg[100];

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

    char* code = malloc(20);
    char* instruction = malloc(20);
    char *delimiter = strchr(msg, " ");

    int length_code = delimiter - msg;
        int length_instruction = strlen(msg) - length_code - 1;

        // Copy parts into respective arrays
        strncpy(code, msg, length_code);
        code[length_code] = '\0';  // Null-terminate the first part

        strncpy(instruction, delimiter + 1, length_instruction);
        instruction[length_instruction] = '\0';

    if(strcmp(code,"hello") == 0){

        if (sendto(sockfd, logo, strlen(logo), 0, (const struct sockaddr*)&clients[client_n], sizeof(clients[client_n])) == -1){
        fprintf(stderr, "Error : sending message");
        free(code);
        free(instruction);
        close(sockfd);
        return 1;
    }  
    }
    free(code);
    free(instruction);
    return 0;
}

// logic
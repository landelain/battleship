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
#include <pthread.h>
#include "client.h"
#define MAX_INPUT_LENGTH 500
#define MAX_MSG_LENGTH 500

int main(int argc, char* argv[]){

    if(argc < 3){
        fprintf(stderr, "Error : missing arguments");
        return 1;
    }

    char* ip_address = argv[1];
    int port = atoi(argv[2]);


    // socket creation
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        fprintf(stderr, "Error : socket creation");
        return 1;
    }
    
    // Create server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);  
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) != 1) {
        fprintf(stderr, "Error : ip address");
        close(sockfd);
        return 1;
    }

    struct client_data *client_data = malloc(sizeof(struct client_data));
    client_data->sockfd = sockfd;
    client_data->dest = server_addr;

    system("clear");

    // initial 0 message 
    char *msg = malloc(MAX_MSG_LENGTH);
    
    if (send_to(client_data, msg)){
        free(msg);
        close(sockfd);
        free(client_data);
        return 1;
    }
    pthread_t listening_thread;
    if (pthread_create(&listening_thread, NULL, listener, client_data) != 0) {
         	perror("Failed to create listening thread");
            free(msg);
            close(sockfd);
            free(client_data);
         	return 1;
	}
    pthread_t reading_thread;
    if (pthread_create(&reading_thread, NULL, reader, client_data) != 0) {
         	perror("Failed to create reading thread");
            free(msg);
            close(sockfd);
            free(client_data);
         	return 1;
	}
    while(1){

        socklen_t server_addr_len = sizeof(struct sockaddr_in);
        ssize_t received_len = recvfrom(sockfd, msg, MAX_MSG_LENGTH - 1, 0, (struct sockaddr *)&server_addr, &server_addr_len);

        if (received_len == -1) {
            fprintf(stderr, "Error : getting back message");
            close(sockfd);
            free(msg);
            free(client_data);
            return 1;
        }

        msg[received_len] = '\0';

        // print the message

        fprintf(stderr, "%s", msg);

        // get input from user
        char *input = malloc(MAX_INPUT_LENGTH);

        fprintf(stderr, "\nserver: waiting for input...\n>");

        if(fgets(input, sizeof(input), stdin) == NULL){
            fprintf(stderr, "Error : reading input\n");
            close(sockfd);
            free(msg);
            free(client_data);
            return 1;
        }

        input[strcspn(input, "\n")] = '\0';
        handle_get(input, client_data);
    }
    close(sockfd);
    free(msg);
    free(client_data);
    return 0;
}

/*
Reader thread:
    constantly reads from input, and calls the necessary functions to process what is read
*/
void* reader(void* data){
    struct client_data *client_data = (struct client_data *) data;
    char* line = malloc(MAX_INPUT_LENGTH);
    if (line == NULL) {
        fprintf(stderr, "ERROR [listener]: Memory allocation failed\n");
        free(line);
        return NULL;
    }

    int msg_len = getline(&line, (size_t*)MAX_INPUT_LENGTH, stdin);
    free(line);
    return NULL;
}
/*
Listener thread:
    constantly receives from server, and calls the necessary functions to process what is received
*/
void* listener(void* data){
    struct client_data *client_data = (struct client_data *) data;
    char* msg = malloc(MAX_MSG_LENGTH);
    if (msg == NULL) {
        fprintf(stderr, "ERROR [listener]: Memory allocation failed\n");
        return NULL;
    }
    socklen_t server_addr_len = sizeof(struct sockaddr_in);
    ssize_t received_len = recvfrom(client_data->sockfd, msg, MAX_MSG_LENGTH - 1, 0, (struct sockaddr *)&client_data->dest, &server_addr_len);
    if (received_len == -1) {
        fprintf(stderr, "ERROR [listener]: %s", strerror(errno));
        free(msg);
        return NULL;
    }

    msg[received_len] = '\0';

    // print the message
    fprintf(stderr, "Received: %s\n", msg);
    free(msg);
    return NULL;
}


int handle_get(char* input, struct client_data *client_data){
    char* str1 = "cmd";
    char* str2 = "msg";

    // if the player wants to use a command / cmd

    if(strcmp(input,str1) == 0){
        fprintf(stderr, "server: enter command\n>");

        if(fgets(input, sizeof(input), stdin) == NULL){
            fprintf(stderr, "Error : reading input command\n");
            return 1;
        }

    }

    // if the player wants to send a message to the other / msg

    if(strcmp(input, str2) == 0){
        fprintf(stderr, "server: enter message\n>");

        if(fgets(input, sizeof(input), stdin) == NULL){
            fprintf(stderr, "Error : reading input command\n");
            return 1;
        }

        if (send_to(client_data, input)){
            return 1;
        }

    }
    else{
        return 1;
    }


    return 0;
}

/*
Use this to send messages to server

We might need to use MUTEX at some point since it'd be possible to try to send messages from different threads at the same time
*/
int send_to(struct client_data *client_data, char* msg){
    if (sendto(client_data->sockfd, msg, strlen(msg), 0, (const struct sockaddr*)&client_data->dest, sizeof(client_data->dest)) == -1){
        fprintf(stderr, "Error : sending message");
        close(client_data->sockfd);
        return 1;
    }
    return 0;
}

/*
Use this to print

We might need to use MUTEX at some point since it'd be possible to try to print messages from different threads at the same time
*/
int print_out(char* msg){
    fprintf(stderr, "%s\n",msg);
    return 0;
}


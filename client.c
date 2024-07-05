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


int handle_get(char* input, int sockfd, struct sockaddr_in *server_addr);


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

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(port);  
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) != 1) {
        fprintf(stderr, "Error : ip address");
        close(sockfd);
        return 1;
    }

    system("clear");

    // initial 0 message 

    char msg[500] = {};

    if (sendto(sockfd, msg, strlen(msg), 0, (const struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        fprintf(stderr, "Error : sending message");
        close(sockfd);
        return 1;
    }  

    while(1){

        socklen_t server_addr_len = sizeof(server_addr);
        ssize_t received_len = recvfrom(sockfd, msg, sizeof(msg) - 1, 0, (struct sockaddr *)&server_addr, &server_addr_len);

        if (received_len == -1) {
            fprintf(stderr, "Error : getting back message");
            close(sockfd);
            return 1;
        }

        msg[received_len] = '\0';

        // print the message

        fprintf(stderr, "%s", msg);

        // get input from user
        char input[500];

        fprintf(stderr, "\nserver: waiting for input...\n>");

        if(fgets(input, sizeof(input), stdin) == NULL){
            fprintf(stderr, "Error : reading input\n");
            close(sockfd);
            return 1;
        }

        input[strcspn(input, "\n")] = '\0';
        handle_get(input, sockfd, &server_addr);
    }

    return 0;
}

int handle_get(char* input, int sockfd, struct sockaddr_in *server_addr){
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

        if (sendto(sockfd, input, strlen(input), 0, (const struct sockaddr*)server_addr, sizeof(*server_addr)) == -1){
            fprintf(stderr, "Error : sending message");
            close(sockfd);
            return 1;
        }

    }
    else{
        return 1;
    }


    return 0;
}
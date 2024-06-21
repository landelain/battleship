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

    // initial Hello message 

    char msg[100] = "hello";

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

        printf("%s", msg);

    }

    return 0;
}
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

//network
int port;
int connected = 0; 
struct sockaddr_in client1 = {0};
struct sockaddr_in client2 = {0};

//logic
int size, carriers, battleships, destroyers, submarines; 


int is_new_client(struct sockaddr_in address);
int handle_new_client(struct sockaddr_in address);

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

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr,"Error: bind failed\n");
        close(sockfd);
        return 1;
    }

    printf("\nThe server is listening\n\n");

    while(1){

        struct sockaddr_in temp_client_addr;
        socklen_t temp_client_addr_len = sizeof(temp_client_addr);
        char msg[100];

        ssize_t received_len = recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr *)&temp_client_addr, &temp_client_addr_len);
        if (received_len > 0) {
            msg[received_len] = '\0';
        }

        if(is_new_client(temp_client_addr) == 0){
            if(handle_new_client(temp_client_addr) == -1){
                fprintf(stderr, "Error : Already 2 clients connected");
                return 1;
            }
        }
        


    }
    return 0;
}

int is_new_client(struct sockaddr_in address){

    if(client1.sin_addr.s_addr == address.sin_addr.s_addr && client1.sin_port == address.sin_port){
        return 1;
    }

    if(client2.sin_addr.s_addr == address.sin_addr.s_addr && client2.sin_port == address.sin_port){
        return 2;
    }

    return 0;
}

int handle_new_client(struct sockaddr_in address){

    if(connected < 2){
        connected += 1;
        return 0;
    }
    
    return -1;
}
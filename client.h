struct client_data {
        int sockfd;
	    struct sockaddr_in dest;
};

int main(int, char* []);
void* reader(void *);   // Need to implement logic to process line
void* listener(void *); // Need to implement logic to process messages
int send_to(struct client_data*, char*);    // ok
int print_out();    // Logic needs to be worked on
int handle_get(char*, struct client_data*); // idk

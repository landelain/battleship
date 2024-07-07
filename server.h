
//network
int port;
int connected = 0; 
struct sockaddr_in clients[2];
//logic
int size, carriers, battleships, destroyers, submarines; 


int main(int, char* []);

int is_new_client(struct sockaddr_in);
int handle_new_client(struct sockaddr_in);
int process_demand(int, char*, int);
int start_game();
void* game_control(void*);
void* listener(void*);
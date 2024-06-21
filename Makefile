
CLIENT_TARGET = client
SERVER_TARGET = server

CLIENT_OBJS = client.o
SERVER_OBJS = server.o

REBUILDABLES = $(CLIENT_OBJS) $(SERVER_OBJS) $(CLIENT_TARGET) $(SERVER_TARGET)

all: $(CLIENT_TARGET) $(SERVER_TARGET)

clean:
	rm -f $(REBUILDABLES)

$(CLIENT_TARGET): $(CLIENT_OBJS)
	cc -g  -o $@ $^

$(SERVER_TARGET): $(SERVER_OBJS)
	cc -g -o $@ $^

%.o : %.c
	cc -g  -Wall -o $@ -c $<

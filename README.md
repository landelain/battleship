# battleship

./client <IPAddress> <Port>
./server <Port> <Size int> <carriers int> <battleships int> <destroyers int> <submarines int>


Message format:
    TXT text_string
    FYI your_board opponent_board
    MYM
    MOV row column
    END int

TXT messages:
    - "Hello" sent by client when joining server
    - "Welcome" sent by server when accepting client (includes logo)
    - "Move result" sent by server when client makes a move

FYI messages:
    - Sent by server to client, includes necessary board information

MYM messages:
    - Sent by server to client: requests a move from player

MOV messages:
    - Sent by client to server: contains player's move

END messages:
    - Sent by server to client: ends game, informs winner

Board format:
    - Only needs to contain information to print it:
        0: water
        1: ship
    Array of 0s and 1s
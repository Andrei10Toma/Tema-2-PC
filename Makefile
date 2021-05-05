SUBSCRIBER_FILE = subscriber.cpp
SERVER_FILE = server.cpp
CC = g++
FLAGS = -Wall -Wextra -g
PORT = 1234
ID1 = AB512743
ID2 = AB512744
ID3 = AB512745YZ
IP = 127.0.0.1

all: server subscriber

server: server.cpp
	$(CC) $(SERVER_FILE) -o server $(FLAGS)

subscriber: subscriber.cpp
	$(CC) $(SUBSCRIBER_FILE) -o subscriber $(FLAGS)

run_server:
	./server $(PORT)

run_subscriber_1:
	./subscriber $(ID1) $(IP) $(PORT)

run_subscriber_2:
	./subscriber $(ID2) $(IP) $(PORT)

run_subscriber_3:
	./subscriber $(ID3) $(IP) $(PORT)

clean:
	rm -rf server subscriber

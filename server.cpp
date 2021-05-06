#include <bits/stdc++.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "helper.h"

using namespace std;

void remove_from_vector(vector<pair<string, bool>>& v, string topic_name) {
    unsigned int i;
    for (i = 0; i < v.size(); i++) {
        if (v[i].first.compare(topic_name) == 0) {
            break;
        }
    }
    v.erase(v.begin() + i);
}

void send_message_from_queue(int client, struct message *message_header, 
    void *msg, struct sockaddr_in * udp_addr) {
        int announce = 0, ret;
        ret = send(client, &announce, sizeof(int), 0);
        DIE(ret < 0, "send announce from queue");
        ret = send(client, udp_addr, sizeof(struct sockaddr_in), 0);
        DIE(ret < 0, "send udp_addr from queue");
        ret = send(client, message_header, 51, 0);
        DIE(ret < 0, "send header from queue");
        if (message_header->data_type == 0) {
            ret = send(client, (struct message_int *) msg, 5, 0);
            DIE(ret < 0, "send message from queue INT");
        } else if (message_header->data_type == 1) {
            ret = send(client, (struct message_short_real *)msg, 2, 0);
            DIE(ret < 0, "send message from queue SHORT_REAL");
        } else if (message_header->data_type == 2) {
            ret = send(client, (struct message_float *)msg, 6, 0);
            DIE(ret < 0, "send message from queue FLOAT");
        } else if (message_header->data_type == 3) {
            ret = send(client, (struct message_string *)msg, 
                sizeof(struct message_string), 0);
            DIE(ret < 0, "send message from queue STRING");
        }
    }

void send_message_int(unordered_map<string, vector<pair<string, bool>>> 
    sub_topics, unordered_map<string, int> clients_socket, 
    struct message_int *msg, struct message *message_header, 
    struct sockaddr_in * udp_addr, unordered_map<string, queue<
    pair<struct sockaddr_in *, pair<struct message *, void *>>>>& wait_msg) {
    int ret;
    for (auto entry : sub_topics) {
        for (auto& topic : entry.second) {
            if (topic.first.compare(message_header->topic_name) == 0) {
                int announce = 0;
                if (clients_socket.find(entry.first) != clients_socket.end()) {
                    ret = send(clients_socket[entry.first], &announce, 
                        sizeof(int), 0);
                    DIE(ret < 0, "send announce INT");
                    ret = send(clients_socket[entry.first], udp_addr,
                        sizeof(struct sockaddr_in), 0);
                    DIE(ret < 0, "send udp_addr INT");
                    ret = send(clients_socket[entry.first], message_header,
                        51, 0);
                    DIE(ret < 0, "send header INT");
                    ret = send(clients_socket[entry.first], msg, 5, 0);
                    DIE(ret < 0, "send message INT");
                } else if (topic.second == true) {
                    wait_msg[entry.first].push(make_pair(udp_addr, 
                        make_pair(message_header, msg)));
                }
            }
        }
    }
}

void send_message_float(unordered_map<string, vector<pair<string, bool>>> 
    sub_topics, unordered_map<string, int> clients_socket, struct message_float
    *msg, struct message *message_header, struct sockaddr_in * udp_addr, 
    unordered_map<string, queue<pair<struct sockaddr_in *, 
    pair<struct message *, void *>>>>& wait_msg) {
    int ret;
    for (auto entry : sub_topics) {
        for (auto& topic : entry.second) {
            if (topic.first.compare(message_header->topic_name) == 0) {
                int announce = 0;
                if (clients_socket.find(entry.first) != clients_socket.end()) {
                    ret = send(clients_socket[entry.first], &announce, 
                        sizeof(int), 0);
                    DIE(ret < 0, "send announce FLOAT");
                    ret = send(clients_socket[entry.first], udp_addr, 
                        sizeof(struct sockaddr_in), 0);
                    DIE(ret < 0, "send udp_addr FLOAT");
                    ret = send(clients_socket[entry.first], 
                        message_header, 51, 0);
                    DIE(ret < 0, "send header FLOAT");
                    ret = send(clients_socket[entry.first], msg, 6, 0);
                    DIE(ret < 0, "send message FLOAT");
                } else if (topic.second == true) {
                    wait_msg[entry.first].push(make_pair(udp_addr, 
                        make_pair(message_header, msg)));
                }
            }
        }
    }
}

void send_message_short_real(unordered_map<string, vector<pair<string, bool>>>
    sub_topics, unordered_map<string, int> clients_socket, 
    struct message_short_real *msg, struct message *message_header, 
    struct sockaddr_in * udp_addr, unordered_map<string, queue<
    pair<struct sockaddr_in *, pair<struct message *, void *>>>>& wait_msg) {
    int ret;
    for (auto entry : sub_topics) {
        for (auto& topic : entry.second) {
            if (topic.first.compare(message_header->topic_name) == 0) {
                int announce = 0;
                if (clients_socket.find(entry.first) != clients_socket.end()) {
                    ret = send(clients_socket[entry.first], &announce,
                        sizeof(int), 0);
                    DIE(ret < 0, "send announce SHORT_REAL");
                    ret = send(clients_socket[entry.first], udp_addr, 
                        sizeof(struct sockaddr_in), 0);
                    DIE(ret < 0, "send udp_addr SHORT_REAL");
                    ret = send(clients_socket[entry.first], message_header,
                        51, 0);
                    DIE(ret < 0, "send header SHORT_REAL");
                    ret = send(clients_socket[entry.first], msg, 2, 0);
                    DIE(ret < 0, "send message SHORT_REAL");
                } else if (topic.second == true) {
                    wait_msg[entry.first].push(make_pair(udp_addr, 
                        make_pair(message_header, msg)));
                }
            }
        }
    }
}

void send_message_string(unordered_map<string, vector<pair<string, bool>>>
    sub_topics, unordered_map<string, int> clients_socket, 
    struct message_string *msg, struct message *message_header, 
    struct sockaddr_in * udp_addr, unordered_map<string, queue<
    pair<struct sockaddr_in *, pair<struct message *, void *>>>>& wait_msg) {
    int ret;
    for (auto entry : sub_topics) {
        for (auto& topic : entry.second) {
            if (topic.first.compare(message_header->topic_name) == 0) {
                // client is subscribed to the topic
                int announce = 0;
                if (clients_socket.find(entry.first) != clients_socket.end()) {
                    // if client is connected the message is sent
                    // send announce = 0 to announce that a message from udp is
                    // coming
                    ret = send(clients_socket[entry.first], &announce, 
                        sizeof(int), 0);
                    DIE(ret < 0, "send announce STRING");
                    // send the udp information (ip and the port of the udp 
                    // client)
                    ret = send(clients_socket[entry.first], udp_addr, 
                        sizeof(struct sockaddr_in), 0);
                    DIE(ret < 0, "send udp_addr STRING");
                    // send the message header that has the type of the data
                    // and the topic name
                    ret = send(clients_socket[entry.first], message_header, 
                        51, 0);
                    DIE(ret < 0, "send header STRNG");
                    // send the message value that has type string
                    ret = send(clients_socket[entry.first], msg, 
                        sizeof(struct message_string), 0);
                    DIE(ret < 0, "send message STRING");
                } else if (topic.second == true) {
                    // if client is not connected and sf is true put it in queue
                    // and send the messages that are in the queue when the
                    // client reconnects
                    wait_msg[entry.first].push(make_pair(udp_addr, 
                        make_pair(message_header, msg)));                    
                }
            }
        }
    }
}

void send_message (struct message* header, char *buffer, 
    unordered_map<string, vector<pair<string, bool>>> clients_topic, 
    unordered_map<string, int> clients, struct sockaddr_in * udp_addr,
    unordered_map<string, queue<pair<struct sockaddr_in *, 
    pair<struct message *, void *>>>>& clients_wait_msg) {
    if (header->data_type == 0) {
        struct message_int *mi = (struct message_int *)calloc(1, 
            sizeof(struct message_int));
        DIE(mi == NULL, "calloc INT failed");
        memcpy(mi, buffer + 51, sizeof(struct message_int));
        send_message_int(clients_topic, clients, mi, header, udp_addr, 
            clients_wait_msg);
    } else if (header->data_type == 1) {
        struct message_short_real *msr = (struct message_short_real *)calloc(1, 
            sizeof(struct message_short_real));
        DIE(msr == NULL, "calloc SHORT_REAL failed");
        memcpy(msr, buffer + 51, sizeof(struct message_short_real));
        send_message_short_real(clients_topic, clients, msr, header, udp_addr, 
            clients_wait_msg);
    } else if (header->data_type == 2) {
        struct message_float *mf = (struct message_float *)calloc(1, 
            sizeof(struct message_float));
        DIE(mf == NULL, "calloc FLOAT failed");
        memcpy(mf, buffer + 51, sizeof(struct message_float));
        send_message_float(clients_topic, clients, mf, header, udp_addr, 
            clients_wait_msg);
    } else if (header->data_type == 3) {
        struct message_string *ms = (struct message_string *)calloc(1, 
            sizeof(struct message_string));
        DIE(ms == NULL, "calloc STRING failed");
        memcpy(ms, buffer + 51, sizeof(struct message_string));
        send_message_string(clients_topic, clients, ms, header, udp_addr, 
            clients_wait_msg);
    }
}

void close_server(int fdmax) {
    for (int i = 5; i <= fdmax; i++) {
        int announce = 1;
        send(i, &announce, sizeof(int), 0);
        send(i, "exit", 4, 0);
    }
}

void disconnect_client(int tcp_sockfd, char *id_recv) {
    int announce = 1;
    send(tcp_sockfd, &announce, sizeof(int), 0);
    send(tcp_sockfd, "exit", 4, 0);
    close(tcp_sockfd);
    printf("Client %s disconnected.\n", id_recv);
}

void disconnect_client_already_connected(int tcp_sockfd, char *id_recv) {
    int announce = 1;
    send(tcp_sockfd, &announce, sizeof(int), 0);
    send(tcp_sockfd, "exit", 4, 0);
    close(tcp_sockfd);
    printf("Client %s already connected.\n", id_recv);
}

int interpret_command(char *buffer_command, 
    vector<pair<string, bool>>& topics, int &sub) {
    char *command_name = strtok(buffer_command, DELIM);
    if (strcmp(command_name, "subscribe") == 0) {
        char *topic_name = strtok(NULL,  DELIM);
        char *sf_char = strtok(NULL, DELIM);
        char *check = strtok(NULL, DELIM);
        // check if the command is valid
        if (command_name == NULL || topic_name == NULL
            || sf_char == NULL) return -1;
        if (check != NULL) return -1;
        bool sf = (bool) atoi(sf_char);
        if (sf != 0 && sf != 1) return -1;
        bool found_topic = false;
        for (auto &topic : topics) {
            if (topic.first.compare(topic_name) == 0) {
                found_topic = true;
                topic.second = sf;
                break;
            }
        }
        if (!found_topic)
            topics.push_back(make_pair(topic_name, sf));
        int announce = 1;
        // send announce = 1 so the client knows will be an action message from
        // the server
        send(sub, &announce, sizeof(int), 0);
        // send the confirmation that the client was subscribed to the topic
        send(sub, "ACK subscribe", 14, 0);
    } else if (strcmp(command_name, "unsubscribe") == 0) {
        char *topic_name = strtok(NULL, DELIM);
        char *check = strtok(NULL, DELIM);
        // check if the command is valid
        if (topic_name == NULL) return -1;
        if (check != NULL) return -1;
        remove_from_vector(topics, topic_name);
        int announce = 1;
        // send announce = 1 so the client knows will be an action message from
        // the server
        send(sub, &announce, sizeof(int), 0);
        // send the confirmation that the client was unsubscribed from the topic
        send(sub, "ACK unsubscribe", 16, 0);
    }
    return 1;
}

int main(int argc, char *argv[]) {
    DIE(argc != 2, "Format is ./server {PORT}");
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);
    int connect_sockfd, tcp_sockfd, udp_sockfd, port;
    struct sockaddr_in serv_addr, sub_addr;
    socklen_t sublen;
    int ret, flag = 1, i;

    fd_set read_fds;
    fd_set tmp_fds;
    int fdmax;
    char buffer[1551], buffer_command[150];

    // map from socket to their id  
    unordered_map<int, string> clients_socket; 
    // connected clients with their id and socket
    unordered_map<string, int> clients;
    // subscribed topics for each client
    unordered_map<string, vector<pair<string, bool>>> clients_topic;
    // map for awaiting messages for the clients (messages sent while the 
    // subscriber is disconnected)
    unordered_map<string, queue<pair<struct sockaddr_in *, 
        pair<struct message *, void *>>>> clients_wait_msg;
    char command[20]; memset(command, 0, 20);

    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    connect_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(connect_sockfd < 0, "tcp connection socket");
    ret = setsockopt(connect_sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, 
        sizeof(int));
    DIE(ret < 0, "setsockopt tcp socket");
    udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    DIE(udp_sockfd < 0, "udp connection socket");
    flag = 1;

    port = atoi(argv[1]);

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // bind the socket for the tcp clients
    ret = bind(connect_sockfd, (struct sockaddr *)&serv_addr, 
        sizeof(struct sockaddr));
    DIE(ret < 0, "bind error tcp socket");

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // bind the socket for the udp clients
    ret = bind(udp_sockfd, (struct sockaddr *)&serv_addr, 
        sizeof(struct sockaddr));
    DIE(ret < 0, "bind error udp socket");

    ret = listen(connect_sockfd, 10);

    // the new descriptors (socket for the connection of the clients and socket
    // for receiving messages from udp clients) are added to the set 
    FD_SET(connect_sockfd, &read_fds);
    FD_SET(udp_sockfd, &read_fds);
    // add the standard input to the set
    FD_SET(STDIN_FILENO, &read_fds);
    fdmax = udp_sockfd;

    while (true) {
        tmp_fds = read_fds;
        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "server select");

        // receive something from standard input
        if (FD_ISSET(STDIN_FILENO, &tmp_fds)) {
            memset(command, 0, 20);
            fgets(command, 20, stdin);
            if (strncmp(command, "exit", 4) == 0) {
                close_server(fdmax);
                break;
            }
        } else if (FD_ISSET(connect_sockfd, &tmp_fds)) {
            // a new subscriber is trying to connect
            sublen = sizeof(sub_addr);
            tcp_sockfd = accept(connect_sockfd, (struct sockaddr *)&sub_addr, 
                &sublen);
            DIE(tcp_sockfd < 0, "accept connection from tcp client");
            int flag = 1;
            // Deactivate Neagle
            ret = setsockopt(tcp_sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag,
                sizeof(int));
            DIE(ret < 0, "setsockopt error");
            // receive the id of the subscriber
            char id_recv[11];
            ret = recv(tcp_sockfd, id_recv, 11, 0);
            if (ret <= 0) {
                disconnect_client(tcp_sockfd, id_recv);
                continue;
            }
            if (clients.find(id_recv) != clients.end()) {
                disconnect_client_already_connected(tcp_sockfd, id_recv);
                continue;
            }
            FD_SET(tcp_sockfd, &read_fds);
            if (tcp_sockfd > fdmax) {
                fdmax = tcp_sockfd;
            }
            printf("New client %s connected from %s:%d.\n", id_recv,
                inet_ntoa(sub_addr.sin_addr), ntohs(sub_addr.sin_port));
            clients.insert(make_pair(id_recv, tcp_sockfd));
            clients_socket.insert(make_pair(tcp_sockfd, id_recv));
            if (clients_topic.find(id_recv) == clients_topic.end())
                clients_topic.insert(make_pair(id_recv, 
                    vector<pair<string, bool>> ()));
            if (clients_wait_msg.find(id_recv) == clients_wait_msg.end())
                clients_wait_msg.insert(make_pair(id_recv, queue<pair<
                    struct sockaddr_in *, pair<struct message *, void *>>>()));
            else {
                // when the client is connecting send to him the messages from
                // the queue
                while (!clients_wait_msg[id_recv].empty()) {
                    pair<struct sockaddr_in *, pair<struct message *, void *>>
                        send_msg = clients_wait_msg[id_recv].front();
                    send_message_from_queue(clients[id_recv], 
                        send_msg.second.first, send_msg.second.second, 
                        send_msg.first);
                    free(send_msg.first);
                    free(send_msg.second.first);
                    free(send_msg.second.second);
                    clients_wait_msg[id_recv].pop();
                }
            }
        } else if (FD_ISSET(udp_sockfd, &tmp_fds)) {
            socklen_t length = sizeof(struct sockaddr_in);
            memset(buffer, 0, 1551);
            struct sockaddr_in *udp_addr = (struct sockaddr_in *)calloc(1, 
                sizeof(struct sockaddr_in));
            DIE(udp_addr == NULL, "calloc udp failed");
            ret = recvfrom(udp_sockfd, buffer, 1551, 0, 
                (struct sockaddr *)udp_addr, &length);
            if (ret < 0) continue;
            struct message *header = (struct message *)calloc(1, 
                sizeof(struct message));
            DIE(header == NULL, "calloc header failed");
            memcpy(header, buffer, sizeof(struct message));
            send_message(header, buffer, clients_topic, clients, udp_addr,
                clients_wait_msg);
        } else {
            for (i = 5; i <= fdmax; i++) {
                if (FD_ISSET(i, &tmp_fds)) {
                    memset(buffer_command, 0, 150);
                    ret = recv(i, buffer_command, 150, 0);
                    DIE(ret < 0, "receive error");
                    if (ret == 0) {
                        // close connection
                        cout << "Client " << clients_socket[i] << 
                            " disconnected." << endl;
                        close(i);
                        FD_CLR(i, &read_fds);
                        clients.erase(clients_socket[i]);
                        clients_socket.erase(i);
                    } else {
                        ret = interpret_command(buffer_command, 
                            clients_topic[clients_socket[i]], i);
                        if (ret == -1) continue;
                    }
                }
            }
        }
    }

    close(connect_sockfd);
    close(udp_sockfd);

    return 0;
}

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
        int announce = 0;
        send(client, &announce, sizeof(int), 0);
        send(client, udp_addr, sizeof(struct sockaddr_in), 0);
        send(client, message_header, 51, 0);
        if (message_header->data_type == 0) {
            send(client, (struct message_int *) msg, 5, 0);
        } else if (message_header->data_type == 1) {
            send(client, (struct message_short_real *)msg, 2, 0);
        } else if (message_header->data_type == 2) {
            send(client, (struct message_float *)msg, 6, 0);
        } else if (message_header->data_type == 3) {
            send(client, (struct message_string *)msg, sizeof(struct message_string), 0);
        }
    }

void send_message_int(unordered_map<string, vector<pair<string, bool>>> sub_topics, 
    unordered_map<string, int> clients_socket, struct message_int *msg,
    struct message *message_header, struct sockaddr_in * udp_addr, 
    unordered_map<string, queue<pair<struct sockaddr_in *, pair<struct message *, void *>>>>& wait_msg) {
    for (auto entry : sub_topics) {
        for (auto& topic : entry.second) {
            if (topic.first.compare(message_header->topic_name) == 0) {
                int announce = 0;
                if (clients_socket.find(entry.first) != clients_socket.end()) {
                    send(clients_socket[entry.first], &announce, sizeof(int), 0);
                    send(clients_socket[entry.first], udp_addr, sizeof(struct sockaddr_in), 0);
                    send(clients_socket[entry.first], message_header, 51, 0);
                    send(clients_socket[entry.first], msg, 5, 0);
                } else if (topic.second == true) {
                    wait_msg[entry.first].push(make_pair(udp_addr, make_pair(message_header, msg)));
                }
            }
        }
    }
}

void send_message_float(unordered_map<string, vector<pair<string, bool>>> sub_topics, 
    unordered_map<string, int> clients_socket, struct message_float *msg,
    struct message *message_header, struct sockaddr_in * udp_addr, 
    unordered_map<string, queue<pair<struct sockaddr_in *, pair<struct message *, void *>>>>& wait_msg) {
    for (auto entry : sub_topics) {
        for (auto& topic : entry.second) {
            if (topic.first.compare(message_header->topic_name) == 0) {
                int announce = 0;
                if (clients_socket.find(entry.first) != clients_socket.end()) {
                    send(clients_socket[entry.first], &announce, sizeof(int), 0);
                    send(clients_socket[entry.first], udp_addr, sizeof(struct sockaddr_in), 0);
                    send(clients_socket[entry.first], message_header, 51, 0);
                    send(clients_socket[entry.first], msg, 6, 0);
                } else if (topic.second == true) {
                    wait_msg[entry.first].push(make_pair(udp_addr, make_pair(message_header, msg)));
                }
            }
        }
    }
}

void send_message_short_real(unordered_map<string, vector<pair<string, bool>>> sub_topics, 
    unordered_map<string, int> clients_socket, struct message_short_real *msg,
    struct message *message_header, struct sockaddr_in * udp_addr,
    unordered_map<string, queue<pair<struct sockaddr_in *, pair<struct message *, void *>>>>& wait_msg) {
    for (auto entry : sub_topics) {
        for (auto& topic : entry.second) {
            if (topic.first.compare(message_header->topic_name) == 0) {
                int announce = 0;
                if (clients_socket.find(entry.first) != clients_socket.end()) {
                    send(clients_socket[entry.first], &announce, sizeof(int), 0);
                    send(clients_socket[entry.first], udp_addr, sizeof(struct sockaddr_in), 0);
                    send(clients_socket[entry.first], message_header, 51, 0);
                    send(clients_socket[entry.first], msg, 2, 0);
                } else if (topic.second == true) {
                    wait_msg[entry.first].push(make_pair(udp_addr, make_pair(message_header, msg)));
                }
            }
        }
    }
}

void send_message_string(unordered_map<string, vector<pair<string, bool>>> sub_topics, 
    unordered_map<string, int> clients_socket, struct message_string *msg,
    struct message *message_header, struct sockaddr_in * udp_addr,
    unordered_map<string, queue<pair<struct sockaddr_in *, pair<struct message *, void *>>>>& wait_msg) {
    for (auto entry : sub_topics) {
        for (auto& topic : entry.second) {
            if (topic.first.compare(message_header->topic_name) == 0) {
                int announce = 0;
                if (clients_socket.find(entry.first) != clients_socket.end()) {
                    send(clients_socket[entry.first], &announce, sizeof(int), 0);
                    send(clients_socket[entry.first], udp_addr, sizeof(struct sockaddr_in), 0);
                    send(clients_socket[entry.first], message_header, 51, 0);
                    send(clients_socket[entry.first], msg, sizeof(struct message_string), 0);
                } else if (topic.second == true) {
                    wait_msg[entry.first].push(make_pair(udp_addr, make_pair(message_header, msg)));                    
                }
            }
        }
    }
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
    // map for awaiting messages for the clients
    unordered_map<string, queue<pair<struct sockaddr_in *, pair<struct message *, void *>>>> clients_wait_msg;
    char command[20]; memset(command, 0, 20);

    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    connect_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(connect_sockfd < 0, "tcp connection socket");
    ret = setsockopt(connect_sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
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
    ret = bind(connect_sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
    DIE(ret < 0, "bind error tcp socket");

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // bind the socket for the udp clients
    ret = bind(udp_sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
    DIE(ret < 0, "bind error udp socket");

    ret = listen(connect_sockfd, 10);

    FD_SET(connect_sockfd, &read_fds);
    FD_SET(udp_sockfd, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    fdmax = udp_sockfd;

    while (true) {
        tmp_fds = read_fds;

        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "server select");

        if (FD_ISSET(STDIN_FILENO, &tmp_fds)) {
            memset(command, 0, 20);
            fgets(command, 20, stdin);
            if (strncmp(command, "exit", 4) == 0) {
                for (int i = 5; i <= fdmax; i++) {
                    int announce = 1;
                    send(i, &announce, sizeof(int), 0);
                    send(i, "exit", 4, 0);
                }
                break;
            }
        } else if (FD_ISSET(connect_sockfd, &tmp_fds)) {
            sublen = sizeof(sub_addr);
            tcp_sockfd = accept(connect_sockfd, (struct sockaddr *)&sub_addr, &sublen);
            DIE(tcp_sockfd < 0, "accept connection from tcp client");
            // receive the id od the subscriber
            char id_recv[11];
            ret = recv(tcp_sockfd, id_recv, 11, 0);
            if (ret <= 0) {
                int announce = 1;
                send(tcp_sockfd, &announce, sizeof(int), 0);
                send(tcp_sockfd, "exit", 4, 0);
                close(tcp_sockfd);
                printf("Client %s disconnected.\n", id_recv);
                continue;
            }
            if (clients.find(id_recv) != clients.end()) {
                int announce = 1;
                send(tcp_sockfd, &announce, sizeof(int), 0);
                send(tcp_sockfd, "exit", 4, 0);
                close(tcp_sockfd);
                printf("Client %s already connected.\n", id_recv);
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
                clients_topic.insert(make_pair(id_recv, vector<pair<string, bool>> ()));
            if (clients_wait_msg.find(id_recv) == clients_wait_msg.end())
                clients_wait_msg.insert(make_pair(id_recv, queue<pair<struct sockaddr_in *, pair<struct message *, void *>>>()));
            else {
                while (!clients_wait_msg[id_recv].empty()) {
                    pair<struct sockaddr_in *, pair<struct message *, void *>> send_msg = clients_wait_msg[id_recv].front();
                    send_message_from_queue(clients[id_recv], 
                        send_msg.second.first, send_msg.second.second, 
                        send_msg.first);
                    clients_wait_msg[id_recv].pop();
                }
            }
        } else if (FD_ISSET(udp_sockfd, &tmp_fds)) {
            socklen_t length = sizeof(struct sockaddr_in);
            memset(buffer, 0, 1551);
            struct sockaddr_in *udp_addr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));
            ret = recvfrom(udp_sockfd, buffer, 1551, 0, (struct sockaddr *)udp_addr, &length);
            if (ret < 0) continue;
            struct message *header = (struct message *)calloc(1, sizeof(struct message));
            memcpy(header, buffer, sizeof(struct message));
            if (header->data_type == 0) {
                struct message_int *mi = (struct message_int *)calloc(1, sizeof(struct message_int));
                memcpy(mi, buffer + 51, sizeof(struct message_int));
                send_message_int(clients_topic, clients, mi, header, udp_addr, clients_wait_msg);
            } else if (header->data_type == 1) {
                struct message_short_real *msr = (struct message_short_real *)calloc(1, sizeof(struct message_short_real));
                memcpy(msr, buffer + 51, sizeof(struct message_short_real));
                send_message_short_real(clients_topic, clients, msr, header, udp_addr, clients_wait_msg);
            } else if (header->data_type == 2) {
                struct message_float *mf = (struct message_float *)calloc(1, sizeof(struct message_float));
                memcpy(mf, buffer + 51, sizeof(struct message_float));
                send_message_float(clients_topic, clients, mf, header, udp_addr, clients_wait_msg);
            } else if (header->data_type == 3) {
                struct message_string *ms = (struct message_string *)calloc(1, sizeof(struct message_string));
                memcpy(ms, buffer + 51, sizeof(struct message_string));
                send_message_string(clients_topic, clients, ms, header, udp_addr, clients_wait_msg);
            }
        } else {
            for (i = 5; i <= fdmax; i++) {
                if (FD_ISSET(i, &tmp_fds)) {
                    memset(buffer_command, 0, 150);
                    ret = recv(i, buffer_command, 150, 0);
                    DIE(ret < 0, "receive error");
                    if (ret == 0) {
                        // close connection
                        cout << "Client " << clients_socket[i] << " disconnected." << endl;
                        close(i);
                        FD_CLR(i, &read_fds);
                        clients.erase(clients_socket[i]);
                        clients_socket.erase(i);
                    } else {
                        char *command_name = strtok(buffer_command, DELIM);
                        if (strcmp(command_name, "subscribe") == 0) {
                            char *topic_name = strtok(NULL,  DELIM);
                            char *sf_char = strtok(NULL, DELIM);
                            char *check = strtok(NULL, DELIM);
                            if (command_name == NULL || topic_name == NULL
                                || sf_char == NULL) continue;
                            if (check != NULL) continue;
                            bool sf = (bool) atoi(sf_char);
                            if (sf != 0 && sf != 1) continue;
                            clients_topic[clients_socket[i]].push_back(make_pair(topic_name, sf));
                            int announce = 1;
                            send(i, &announce, sizeof(int), 0);
                            send(i, "ACK subscribe", 14, 0);
                        } else if (strcmp(command_name, "unsubscribe") == 0) {
                            char *topic_name = strtok(NULL, DELIM);
                            char *check = strtok(NULL, DELIM);
                            if (topic_name == NULL) continue;
                            if (check != NULL) continue;
                            remove_from_vector(clients_topic[clients_socket[i]], topic_name);
                            int announce = 1;
                            send(i, &announce, sizeof(int), 0);
                            send(i, "ACK unsubscribe", 16, 0);
                        }
                    }
                }
            }
        }
    }

    close(connect_sockfd);
    close(udp_sockfd);

    return 0;
}

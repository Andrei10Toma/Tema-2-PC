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

void receive_int(struct message * message_header, struct sockaddr_in * udp_addr,
    int sockfd) {
    struct message_int message_int;
    int ret = recv(sockfd, &message_int, sizeof(struct message_int), 0);
    DIE(ret < 0, "Message int receive");
    if (message_int.sign_octet == 0) {
        printf("%s:%d - %s - INT - %d\n", inet_ntoa(udp_addr->sin_addr), 
        ntohs(udp_addr->sin_port), message_header->topic_name,
        ntohl(message_int.value));
    } else {
        printf("%s:%d - %s - INT - %d\n", inet_ntoa(udp_addr->sin_addr), 
        ntohs(udp_addr->sin_port), message_header->topic_name, 
        -1 * ntohl(message_int.value));
    }
}

void receive_short_real(struct message * message_header,
    struct sockaddr_in * udp_addr, int sockfd) {
    struct message_short_real message_short_real;
    int ret = recv(sockfd, &message_short_real, 
        sizeof(struct message_short_real), 0);
    DIE(ret < 0, "Message short real receive");
    printf("%s:%d - %s - SHORT_REAL - %.2f\n",
    inet_ntoa(udp_addr->sin_addr), ntohs(udp_addr->sin_port), 
    message_header->topic_name, ntohs(message_short_real.value) / (float)100);
}

void receive_float(struct message * message_header, 
    struct sockaddr_in * udp_addr, int sockfd) {
    struct message_float message_float; 
    int ret = recv(sockfd, &message_float, sizeof(struct message_float), 0);
    DIE(ret < 0, "Message float receive");
    if (message_float.sign_octet == 0) {
        printf("%s:%d - %s - FLOAT - %f\n", 
        inet_ntoa(udp_addr->sin_addr), ntohs(udp_addr->sin_port),
        message_header->topic_name, (float) ntohl(message_float.value)
        / pow(10, message_float.exponent));
    } else {
        printf("%s:%d - %s - FLOAT - %f\n", 
        inet_ntoa(udp_addr->sin_addr), ntohs(udp_addr->sin_port),
        message_header->topic_name, (float) (-1) * ntohl(message_float.value)
        / pow(10, message_float.exponent));
    }
}

void receive_string(struct message * message_header, 
    struct sockaddr_in * udp_addr, int sockfd) {
    struct message_string message_string; 
    int ret = recv(sockfd, &message_string, sizeof(struct message_string), 0);
    DIE(ret < 0, "Message string receive");
    printf("%s:%d - %s - STRING - %s\n", inet_ntoa(udp_addr->sin_addr),
    ntohs(udp_addr->sin_port), message_header->topic_name, 
    message_string.value);
}

int main(int argc, char *argv[]) {
    DIE(argc != 4, 
        "Format is ./subscriber {ID_CLIENT} {IP_SERVER} {PORT_SERVER}");
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);
    int sockfd, ret, port;
    struct sockaddr_in serv_addr;
    fd_set read_fds, tmp_fds;
    int fd_max;
    char command[150], buffer[150];

    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "subscriber socket");

    int flag = 1;
    // Deactivate Neagle
    ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag,
        sizeof(int));
    DIE(ret < 0, "setsockopt error");

    port = atoi(argv[3]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    ret = inet_aton(argv[2], &serv_addr.sin_addr);
    DIE(ret == 0, "inet_aton");

    ret = connect(sockfd, (struct sockaddr *) &serv_addr,
        sizeof(struct sockaddr));
    DIE(ret < 0, "subscriber connect");
    // after the connection send the ip of the client
    ret = send(sockfd, argv[1], strlen(argv[1]) + 1, 0);
    DIE(ret < 0, "send the ip");

    // add the socket where the server will send messages (sockfd) and the
    // standard input to the set
    FD_SET(sockfd, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    fd_max = sockfd;

    while (true) {
        tmp_fds = read_fds;
        ret = select(fd_max + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "subscriber select");
        if (FD_ISSET(STDIN_FILENO, &tmp_fds)) {
            memset(command, 0, 150);
            fgets(command, 150, stdin);
            // remove '\n'
            command[strlen(command) - 1] = '\0';
            if (strncmp(command, "exit", 4) == 0) {
                break;
            } else {
                // send the command to the server to interpret it
                ret = send(sockfd, command, 150, 0);
                DIE(ret < 0, "send command error");
            }
        } else if (FD_ISSET(sockfd, &tmp_fds)) {
            memset(buffer, 0, 150);
            int anounce = -1;
            ret = recv(sockfd, &anounce, sizeof(int), 0);
            // receive announce = 0 if the server sends a message from udp
            // receive announce = 1 if the server sends a action message when
            // the subscribe, unsubscribe or a force exit for the client is
            // necesarry.
            DIE(ret < 0, "announce receive");
            if (anounce == 1) {
                ret = recv(sockfd, buffer, 150, 0);
                if (ret < 0) continue;
                if (strncmp(buffer, "exit", 4) == 0) {
                    break;
                } else if (strncmp(buffer, "ACK subscribe", 14) == 0) {
                    printf("Subscribed to topic.\n");
                } else if (strncmp(buffer, "ACK unsubscribe", 16) == 0) {
                    printf("Unsubscribed from topic.\n");
                }
            } else {
                struct sockaddr_in udp_adr;
                // receive the udp socket address
                ret = recv(sockfd, &udp_adr, sizeof(struct sockaddr_in), 0);
                DIE(ret < 0, "Udp receive error");
                struct message message_header;
                // receive the header of the message with the name of the topic
                // and the type of the data
                ret = recv(sockfd, &message_header, sizeof(struct message), 0);
                DIE(ret < 0, "Message header receive");
                if (message_header.data_type == 0) {
                    receive_int(&message_header, &udp_adr, sockfd);
                } else if (message_header.data_type == 1) {
                    receive_short_real(&message_header, &udp_adr, sockfd);
                } else if (message_header.data_type == 2) {
                    receive_float(&message_header, &udp_adr, sockfd);
                } else if (message_header.data_type == 3) {
                    receive_string(&message_header, &udp_adr, sockfd);
                }
            }
        }
    }

    close(sockfd);

    return 0;
}

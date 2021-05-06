#ifndef _HELPERS_H
#define _HELPERS_H 1
#define DELIM " \n"

#include <stdio.h>
#include <stdlib.h>

/**
 * Structure used to extract the header of the topic message from the udp
 * clients. The message header will contain the name of the topic and the type
 * of the data (51 bytes).
 * */
struct message {
    char topic_name[50];
    int8_t data_type;
}__attribute__((packed));

/**
 * Structure used to extract the message that is INT from the topic message sent
 * by the udp client (5 bytes).
 * */
struct message_int {
    int8_t sign_octet;
    uint32_t value;
}__attribute__((packed));

/**
 * Structure used to extract the message that is SHORT_REAL from the topic
 * message sent by the udp client (2 bytes).
 * */
struct message_short_real {
    uint16_t value;
}__attribute__((packed));

/**
 * Structure used to extract the message that is STRING from the topic message
 * sent by the udp client (1500 bytes).
 * */
struct message_string {
    char value[1500];
}__attribute__((packed));

/**
 * Structure used to extract the message that is FLOAT from the topic message
 * sent by the udp client (6 bytes).
 * */
struct message_float {
    int8_t sign_octet;
    uint32_t value;
    uint8_t exponent;
}__attribute__((packed));

#define DIE(assertion, call_description)    \
    do {                                    \
        if (assertion) {                    \
            fprintf(stderr, "(%s, %d): ",   \
            __FILE__, __LINE__);            \
            perror(call_description);       \
            exit(EXIT_FAILURE);             \
        }                                   \
    } while (0)

#endif

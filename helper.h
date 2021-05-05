#ifndef _HELPERS_H
#define _HELPERS_H 1
#define DELIM " \n"

#include <stdio.h>
#include <stdlib.h>

struct message {
    char topic_name[50];
    int8_t data_type;
}__attribute__((packed));

struct message_int {
    int8_t sign_octet;
    uint32_t value;
}__attribute__((packed));

struct message_short_real {
    uint16_t value;
}__attribute__((packed));

struct message_string {
    char value[1500];
}__attribute__((packed));

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
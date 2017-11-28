
#ifndef MSGS_H
#define MSGS_H

#include <string.h>
#include "sem.h"

#define N_MESSAGES 10

typedef struct message {
    int integers[10];
	char* messagebody[2];
} message_t;

typedef struct port {
    message_t messages[N_MESSAGES];
    sem_t mutex;
    sem_t sync_send;
    sem_t sync_receive;
    int send;
    int receive;
} port_t;

port_t ports[100];

void init_port(port_t *port)
{
    memset(&port->messages, 0, sizeof(port->messages));
    init_sem(&port->mutex, 1);
    init_sem(&port->sync_send, N_MESSAGES);
    init_sem(&port->sync_receive, 0);
    port->send = 0;
    port->receive = 0;
}

// Black magic to make the first send/receive set up the global ports
void private_init_send(port_t *port, message_t message);
void (*send)(port_t *port, message_t message) = &private_init_send;

// Black magic to make the first send/receive set up the global ports
message_t private_init_receive(port_t *port);
message_t (*receive)(port_t *port) = &private_init_receive;

void private_send(port_t *port, message_t message)
{
    p(&port->sync_send);
    p(&port->mutex);
    port->messages[port->send] = message;
    port->send = (port->send + 1) % N_MESSAGES;
    v(&port->mutex);
    v(&port->sync_receive);
}

message_t private_receive(port_t *port)
{
    message_t message;
    p(&port->sync_receive);
    p(&port->mutex);
    message = port->messages[port->receive];
    port->receive = (port->receive + 1) % N_MESSAGES;
    v(&port->mutex);
    v(&port->sync_send);
    return message;
}

// Initialize the global ports
void private_init_ports(void)
{
    int num_ports = sizeof(ports) / sizeof(port_t);
    for (int i = 0; i < num_ports; ++i)
        init_port(&ports[i]);
}

// Black magic to make the first send/receive set up the global ports
void private_correct_function_pointers(void)
{
    send = &private_send;
    receive = &private_receive;
}

// Black magic to make the first send/receive set up the global ports
void private_init_send(port_t *port, message_t message)
{
    private_init_ports();
    private_correct_function_pointers();
    send(port, message);
}

// Black magic to make the first send/receive set up the global ports
message_t private_init_receive(port_t *port)
{
    private_init_ports();
    private_correct_function_pointers();
    return receive(port);
}

#endif

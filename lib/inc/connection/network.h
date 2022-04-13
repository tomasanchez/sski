/**
* network.h
*
* @file  API de Networking
* @author Tomás Sánchez
* @since  09.10.2021
*/

#pragma once

#include <poll.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define HOSTNAME "localhost"
#define INTERFACE "enp0s3"

char *
get_ipv4();

void *get_in_addr(struct sockaddr *sa);

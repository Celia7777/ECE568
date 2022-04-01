#ifndef SOCKET
#define SOCKET

#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>

int setup(const char * port);
int toAccept(int socket_fd, std::string &ip);
int trySelect(int socket_fd);
int setupClient(const char* hostname, const char * port);



#endif
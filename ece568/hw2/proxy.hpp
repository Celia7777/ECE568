#include <iostream>
#include <pthread.h>
#include <string>
#include <cstring>
#include <algorithm>
#include <fstream>
#include "server.hpp"
#include "request.hpp"
#include "response.hpp"
#include "client.hpp"
#include "cache.hpp"
#include "thread.h"

class proxy{
    private:
        const char* port_num;
        int id = 0;
        //std::string log("/var/log/erss/proxy.log");
    public:
        proxy(const char* port);
        void start();
        void printContent(char *content);
        static void tryConnect(int server_accept_socket, int connectToServer_fd);
        static void * processRequestFromClient(void * mythread_ptr);
        static void receiveRequestFromClient(int fd, std::string &ans);
        static void postRequest(int client_fd, int server_fd, Request rqst);
        static void getRequest(int client_fd, int server_fd, Request rqst, cache* mycache);
        static std::string receiveAllchunks(int socket_as_server, std::string rqst, int content_length);
        static void revalidate(int proxy_as_client_fd, int proxy_as_server_fd, Request rqst, Response resp, cache* mycache);
        static std::string receiveAllmsg(int fd, std::string msg, bool chunk); 
};
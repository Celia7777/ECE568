#ifndef _THREAD_INFO_H__
#define _THREAD_INFO_H__
#include "cache.hpp"
#include <pthread.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

class Thread{
public:
    cache *mycache;
    int sockect_as_server;

    Thread(cache *mycache, int server_sockect):mycache(mycache), sockect_as_server(server_sockect){}
};

#endif
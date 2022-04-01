#ifndef SERVER_HPP
#define SERVER_HPP

// #include "client.hpp"
// #include "server.hpp"
#include "socket.h"

#include "xml_process.hpp"
#include "parse.hpp"
#include "db_process.hpp"
#include <pqxx/pqxx>
#include <thread>


#define PORT "12345"

// using namespace std;
using namespace pqxx;

void ProcessClientrqst(int connect_to_client_fd);

#endif
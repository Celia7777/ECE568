#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "addrinfo.hpp"

enum client_server_type{
      SERVER = 0,
      CLIENT = 1
};

class socket_wrapper {

private:

  // has-a
  int listener_sockfd; // listen on this socket
  int new_sockfd; // send() and rcv() using this port for each connection
  client_server_type server_client;
  
  
  public:
  // constructor
  socket_wrapper( client_server_type server_client);

  
  void start_server(addrinfo_st * servinfo);
  void start_client(addrinfo_st * servinfo);
  
  void start_socket(addrinfo_st * servinfo) ;
  void create_socket_and_connect();
  bool bind_or_connect(int(*func)(int, const struct sockaddr *, socklen_t), int listener_sockfd, struct addrinfo *p );

  int get_listerner_sockfd();
  
};

#endif

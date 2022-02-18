#include "socket.hpp"
#include "proxy.hpp"
#include "custom_exception.hpp"

  socket_wrapper::socket_wrapper( client_server_type server_client) {
    this->server_client = server_client;
  }
  void socket_wrapper::start_server(addrinfo_st * servinfo) {
  }

  void socket_wrapper::start_client(addrinfo_st * servinfo) {
  }
  
  void socket_wrapper::start_socket(addrinfo_st * servinfo) {

    addrinfo_st *p;
    int yes = 1;
  
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((listener_sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
	  std::cerr << "server: socket";
            continue;
        }

	// lose the pesky "Address already in use" error message
	// by reusing the port
        if (setsockopt(listener_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
	  throw new custom_exception("Error in setsocketopt");
        }

	// bind for server, connect for client
	if(!bind_or_connect(bind, listener_sockfd, p)) {
	  continue;
	}


        break;
    }
    
    if (p == NULL)  {
        throw new custom_exception("Error: server: failed to bind");
    }
}

  void socket_wrapper::create_socket_and_connect() {
  }

  bool socket_wrapper::bind_or_connect(int(*func)(int, const struct sockaddr *, socklen_t), int listener_sockfd, struct addrinfo *p ) {
    bool success = true;
    if (func(listener_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(listener_sockfd);
      std::cerr << "server: bind";
      success = false;
    }
    return success;
  }


int socket_wrapper::get_listerner_sockfd() {
  return listener_sockfd;
}

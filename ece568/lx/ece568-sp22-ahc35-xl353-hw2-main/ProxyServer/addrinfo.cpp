#include "addrinfo.hpp"
#include "proxy.hpp"
addrinfo_wrapper::addrinfo_wrapper() {
  int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // either IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM; // stream socket (not connectionless)
    hints.ai_flags = AI_PASSIVE; // use my IP of local host

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
      std::string err = "getaddrinfo: ";
      err += gai_strerror(rv);
      throw new custom_exception(err);
    }
}

void addrinfo_wrapper::freeServInfo() {
  freeaddrinfo(servinfo); // all done with this structure
  servinfo = NULL;
}

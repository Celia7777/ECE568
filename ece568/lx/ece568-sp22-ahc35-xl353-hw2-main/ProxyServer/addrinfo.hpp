#ifndef ADDRINFO_HPP
#define ADDRINFO_HPP
#include <sys/socket.h>
#include <netdb.h>


typedef struct addrinfo addrinfo_st;

class addrinfo_wrapper {
  //private:
public: // TEMPORARY
  addrinfo_st hints, *servinfo;
  
public:
  // constructor
  addrinfo_wrapper();
  
  // any other methods
  void freeServInfo();
};

#endif

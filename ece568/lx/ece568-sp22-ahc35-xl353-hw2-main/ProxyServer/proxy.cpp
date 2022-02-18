#include "proxy.hpp"
#include "socket.hpp"
#include "addrinfo.hpp"
#include "custom_exception.hpp"


void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int new_fd;  // listen on sock_fd, new connection on new_fd
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    char s[INET6_ADDRSTRLEN];
    
    
    try {
      // Step 1: getaddrinfo()
      addrinfo_wrapper adInfo;
      
      char hostname[128];
      gethostname(hostname, sizeof hostname);
      std::cout << "My hostname: " <<hostname << std::endl ;

      // Step 2: socket() and Step 3: bind() 
      socket_wrapper sock(SERVER);
      sock.start_socket(adInfo.servinfo);
      adInfo.freeServInfo();
      
    
    
    // Step 4: listen()
    
    if (listen(sock.get_listerner_sockfd(), BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    // Step 5: accept()

    
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sock.get_listerner_sockfd(), (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(sock.get_listerner_sockfd()); // child doesn't need the listener

	    char buf[1024];
	    recv(new_fd, buf, 1024, 0);
	    

	    if (send(new_fd, buf, strlen(buf), 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    }
    catch(custom_exception & e) {
      // printing the error msg
      std::cerr << "Exception: " << e.what();
      return EXIT_FAILURE;
    }
    

    return 0;
}

CCFLAGS=--std=gnu++98 -pedantic -Wall -Werror -ggdb3

proxy:  proxy.o socket.o addrinfo.o custom_exception.o
	g++ -o proxy $(CCFLAGS)  proxy.o socket.o addrinfo.o custom_exception.o

%.o: %.cpp proxy.hpp addrinfo.hpp socket.hpp
	g++ -c $(CCFLAGS) $<

clean:
	rm -f *.o  *~ proxy

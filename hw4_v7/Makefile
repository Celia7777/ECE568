CCFLAGS=--std=gnu++98 -pedantic -Wall -Werror -ggdb3
EXTRAFLAGS=-lpqxx -lpq -pthread

all: server client

client: client.o socket.o xml_process.o tinyxml2.o
	g++ -o client $(CCFLAGS) client.o socket.o xml_process.o tinyxml2.o $(EXTRAFLAGS)

server: server.o socket.o xml_process.o tinyxml2.o parse.o db_process.o
	g++ -o server $(CCFLAGS) server.o socket.o xml_process.o tinyxml2.o parse.o db_process.o $(EXTRAFLAGS)

%.o: %.cpp server.hpp socket.h xml_process.hpp tinyxml2.hpp parse.hpp db_process.hpp
	g++ -c $(CCFLAGS) $<

clean:
	rm -f *.o  *~ client server


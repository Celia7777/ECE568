#include "xml_process.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "socket.h"

int main(int argc, char *argv[]){
    if(argc != 3){
        std::cerr<<"client <machine_name> <port_num>"<< std::endl;
        return EXIT_FAILURE;
    }
    //create XML
    std::string ref;
    std::string account_id = "1234567890";
    std::string balance = "1000";
    std::string symbol = "SPY";
    int sym_num = 10000;
    std::string amount = "1000";
    std::string limit = "1000";
    std::string trans_id = "1";
    // flag for <create>, <transactions> and <>
    //generate XML formate for <create>
    createXML(ref, account_id, balance, symbol, sym_num);
    //generate XML formate for <transactions>
    transactionXML(ref, account_id, symbol, amount, limit, trans_id);
    std::cout <<"send test: "<< ref << std::endl;

    const char* domain = argv[1];
    const char* port = argv[2];
    int client_fd = setupClient(domain, port);

    int counter = 0;
    while(1){
        //int len = trySelect(client_fd);
        
        // if(len == 0){
        //     break;
        // }
        std::cout<<"counter: "<< counter<< " send test from client: "<<ref.c_str() << "len: "<<ref.length() +1 <<std::endl;
        counter +=1;
        send(client_fd, ref.c_str(), ref.length() + 1, 0);
        
    }
}




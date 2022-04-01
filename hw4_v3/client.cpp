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
    //create
    std::string ref;
    std::string account_id1 = "1234567890";
    std::string balance1 = "10000";
    std::string symbol1 = "SPY2";
    int sym_num1 = 10000;
    //create
    std::string ref2;
    std::string account_id2 = "88888";
    std::string balance2 = "55555";
    std::string symbol2 = "SPY";
    int sym_num2 = 10000;
    //transaction
    //1
    std::string ref3;
    // std::string account_id1 = "1234567890";
    // std::string symbol1 = "SPY";
    std::string amount1 = "10";
    std::string limit1 = "125";
    std::string trans_id1 = "1";
    //2
    std::string ref4;
    // std::string account_id2 = "88888";
    // std::string symbol2 = "SPY";
    std::string amount2 = "-50";
    std::string limit2 = "120";
    std::string trans_id2 = "1";
    // flag for <create>, <transactions> and <>
    //generate XML formate for <create>
    createXML(ref, account_id1, balance1, symbol1, sym_num1);
    createXML(ref2, account_id2, balance2, symbol2, sym_num2);
    //generate XML formate for <transactions>
    transactionXML(ref3, account_id1, symbol1, amount1, limit1, trans_id1);
    transactionXML(ref4, account_id2, symbol2, amount2, limit2, trans_id2);
    std::cout <<"send test: "<< ref << std::endl;
    std::cout <<"send test: "<< ref2 << std::endl;

    const char* domain = argv[1];
    const char* port = argv[2];
    int client_fd = setupClient(domain, port);

    int counter = 0;
    while(1){
        //int len = trySelect(client_fd);
        
        // if(len == 0){
        //     break;
        // }
        // std::cout<<"counter: "<< counter<< " send test from client: "<<ref.c_str() << "len: "<<ref.length() +1 <<std::endl;
        std::cout<<"counter: "<< counter<< " send test from client: "<<ref2.c_str() << "len: "<<ref2.length() +1 <<std::endl;
        counter +=1;
        send(client_fd, ref.c_str(), ref.length() + 1, 0);
        // send(client_fd, ref2.c_str(), ref2.length() + 1, 0);
        
    }
}


    // ref = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" \
    // "<create><account id=\"1234567890\" balance=\"1000\"/>" \
    // "symbol sym=\"SPY\">" \
    // "<account id=\"1234567890\">100000</acount>" \
    // "</symbol>" \
    // "</create>";




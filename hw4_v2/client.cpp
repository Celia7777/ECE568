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
#include <sstream>

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
    std::string symbol1 = "A";
    int sym_num1 = 10000;
    //create
    std::string ref2;
    std::string account_id2 = "88888";
    std::string balance2 = "55555";
    std::string symbol2 = "A";
    int sym_num2 = 10000;
    //transaction
    //1
    std::string ref3;
    // std::string account_id1 = "1234567890";
    // std::string symbol1 = "SPY";
    std::string amount1 = "500";
    std::string limit1 = "125";
    std::string trans_id1 = "4";
    //2
    std::string ref4;
    // std::string account_id2 = "88888";
    // std::string symbol2 = "SPY";
    std::string amount2 = "5";
    std::string limit2 = "120";
    std::string trans_id2 = "1";
    // flag for <create>, <transactions> and <>
    //generate XML formate for <create>
    createXML(ref, account_id1, balance1, symbol1, sym_num1);
    createXML(ref2, account_id2, balance2, symbol2, sym_num2);
    //generate XML formate for <transactions>
    transactionXML(ref3, account_id1, symbol1, amount1, limit1, trans_id1);
    transactionXML(ref4, account_id2, symbol2, amount2, limit2, trans_id2);
    std::stringstream ss_1;
    ss_1<<ref.length()<<'\n'<<ref;
    ref = ss_1.str();
    std::stringstream ss_2;
    ss_2<<ref2.length()<<'\n'<<ref2;
    ref2 = ss_2.str();
    std::stringstream ss_3;
    ss_3<<ref3.length()<<'\n'<<ref3;
    ref3 = ss_3.str();
    std::stringstream ss_4;
    ss_4<<ref4.length()<<'\n'<<ref4;
    ref4 = ss_4.str();


    const char* domain = argv[1];
    const char* port = argv[2];
    int client_fd = setupClient(domain, port);

    std::vector<std::string> requests;
    requests.push_back(ref);
    requests.push_back(ref2);
    requests.push_back(ref3);
    requests.push_back(ref4);
    
    for(int i = 0; i < requests.size(); i++){
    // while(1){
        //int len = trySelect(client_fd);
        
        // if(len == 0){
        //     break;
        // }
        // std::cout<<"counter: "<< counter<< " send test from client: "<<ref.c_str() << "len: "<<ref.length() +1 <<std::endl;
        //std::cout<<"counter: "<< counter<< " send test from client: "<<requests[i].c_str() << "len: "<<requests[i].length() +1 <<std::endl;
        std::cout <<"send the " <<i<< "th test: "<< requests[i] <<"length is:"<< requests[i].length() + 1<< std::endl;
        send(client_fd, requests[i].c_str(), requests[i].length() + 1, 0);
        //int send_len = send(client_fd, ref.c_str(), ref.length() + 1, 0);
        
        //send(client_fd, ref2.c_str(), ref2.length() + 1, 0);
        
    //}
    }
}


    // ref = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" \
    // "<create><account id=\"1234567890\" balance=\"1000\"/>" \
    // "symbol sym=\"SPY\">" \
    // "<account id=\"1234567890\">100000</acount>" \
    // "</symbol>" \
    // "</create>";




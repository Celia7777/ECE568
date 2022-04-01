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

#include <cstdlib>
#include <ctime>

//generate random test
//<create>
//account_id [0, 100000)
std::string generate_id(unsigned seed){
    // unsigned seed;
    // seed = time(NULL);
    srand(seed);
    //return std::to_string(((rand()% (100000)) + 0));
    return std::to_string(((rand()% (100)) + 0));
}
//balance
double generate_balance(unsigned seed){
    // unsigned seed;
    // seed = time(NULL);
    srand(seed);
    return ((rand()% (200000)) + 1);
}
//symbol
char generate_sym(unsigned seed){
    // unsigned seed;
    // seed = time(NULL);
    srand(seed);
    //int rand_num = rand()%26;
    int rand_num = rand()%10;
    return('A' + rand_num);
}
//amount
double generate_amount(unsigned seed){
    // unsigned seed;
    // seed = time(NULL);
    srand(seed);
    return ((rand()% (200)) + -100);
}
//<transactions>
//price
double generate_price(unsigned seed){
    // unsigned seed;
    // seed = time(NULL);
    srand(seed);
    return ((rand()% (200)) + 0);
}
std::string generate_trans_id(unsigned seed){
    // unsigned seed;
    // seed = time(NULL);
    srand(seed);
    //return std::to_string(((rand()% (100000)) + 0));
    return std::to_string(((rand()% (50)) + 0));
}
//order:0 / query:1 / cancel: 2 [0,3)
int generate_mode(unsigned seed){
    srand(seed);
    return ((rand()% 3) + 0);
}

std::string receiveAllmsg(int fd, std::string msg, int& total){
    int rev_len = 0;
    std::string total_rqst;
    total_rqst.append(msg);
    //std::cout<<"first string: "<<total_rqst<<std::endl;
    
    while(1){
        //std::cout<<"in while now"<<std::endl;
        char rev_msg[65536] = {0};
        rev_len = recv(fd, rev_msg, sizeof(rev_msg), 0);
        //std::cout<<"first receive length: "<<rev_len<<std::endl;
        if(rev_len<=0){
            break;
        }
        std::string rev_msg_str(rev_msg, rev_len);
        //std::cout<<"receive length is "<<rev_len<<" receive message is "<<rev_msg_str<<std::endl;
        
        // std::string rec(rev_msg, rev_len);
        total_rqst.append(rev_msg_str);
        total += rev_len;
        //std::cout<<"chunk total length: "<<total<<std::endl;
    }

    return total_rqst;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        std::cerr<<"client <machine_name> <port_num>"<< std::endl;
        return EXIT_FAILURE;
    }
    const char* domain = argv[1];
    const char* port = argv[2];
    int client_fd = setupClient(domain, port);

    int times = 10;
    unsigned seed = time(NULL);
    //randomly generate requests
    for(int i = 0; i < times; i++){
        seed += i;
        std::string id = generate_id(seed);
        std::cout<<"id: "<<id<<std::endl;
        double balance = generate_balance(seed);
        std::cout<<"balance: "<<balance<<std::endl;
        std::stringstream ss_balance;
        ss_balance << balance;
        std::string balance_str = ss_balance.str();

        char symbol = generate_sym(seed);
        std::cout<<"symbol: "<<symbol<<std::endl;
        std::string symbol_str = std::to_string(symbol);
        double amount = generate_amount(seed);
        std::cout<<"amount: "<<amount<<std::endl;
        std::stringstream ss_amount;
        ss_amount << amount;
        std::string amount_str = ss_amount.str();

        double price = generate_price(seed);
        std::cout << "price: " << price << std::endl;
        std::stringstream ss_price;
        ss_price << price;
        std::string price_str = ss_price.str();

        std::string trans_id = generate_trans_id(seed);
        std::cout<< "trans_id: " << trans_id <<std::endl;

        int mode = generate_mode(seed);
        std::cout << "mode: " << mode << std::endl;


        //double price = generate_price();
        std::string ref;
        //create <create> XML
        if (i< (times/2)){
            createXML(ref, id, balance_str, symbol_str, amount);
        }
        else{
            transactionXML(ref, id, symbol_str, amount_str, price_str, trans_id, mode);
        }        
        //insert the length of this xml
        std::stringstream ss;
        ss<<ref.length()<<'\n'<<ref;
        ref = ss.str();
        std::cout<<"client sent: "<<ref<<std::endl;
        //generate the vector of the xml
        std::vector<std::string> requests;
        requests.push_back(ref);
        send(client_fd, ref.c_str(), ref.length() + 1, 0);
        std::cout<<"counter: "<<i<<std::endl;

        //receive response
        char resps[65536] = {0};
        memset(resps, 0, sizeof(resps));
        int resp_len = recv(client_fd, resps, sizeof(resps), 0);
        std::string response;
        response.assign(resps, resp_len);
        //response = receiveAllmsg(client_fd, response, resp_len);
        std::cout<<"client received response: "<<response<<std::endl;
    }

    // //receive response
    // while(1){
    //     char resps[65536] = {0};
    //     memset(resps, 0, sizeof(resps));

    //     int resp_len = recv(client_fd, resps, sizeof(resps), 0);
    //     if(resp_len == 0){
    //         break;
    //     }
    //     if(resp_len==-1){
    //         std::cerr << "Error in receive first response from server" << std::endl;
    //         return EXIT_FAILURE;
    //     }
    //     std::string response;
    //     response.assign(resps, resp_len);
    //     response = receiveAllmsg(client_fd, response, resp_len);
    //     std::cout<<"client received response: "<<response<<std::endl;
    // }

    

    // //create XML
    // //create
    // std::string ref;
    // std::string account_id1 = "1234567890";
    // std::string balance1 = "10000";
    // std::string symbol1 = "A";
    // int sym_num1 = 10000;
    // //create
    // std::string ref2;
    // std::string account_id2 = "88888";
    // std::string balance2 = "55555";
    // std::string symbol2 = "A";
    // int sym_num2 = 10000;
    // //transaction
    // //1
    // std::string ref3;
    // // std::string account_id1 = "1234567890";
    // // std::string symbol1 = "SPY";
    // std::string amount1 = "500";
    // std::string limit1 = "125";
    // std::string trans_id1 = "4";
    // //2
    // std::string ref4;
    // // std::string account_id2 = "88888";
    // // std::string symbol2 = "SPY";
    // std::string amount2 = "-5";
    // std::string limit2 = "120";
    // std::string trans_id2 = "1";
    // flag for <create>, <transactions> and <>
    //generate XML formate for <create>
    // createXML(ref, account_id1, balance1, symbol1, sym_num1);
    // createXML(ref2, account_id2, balance2, symbol2, sym_num2);
    // //generate XML formate for <transactions>
    // transactionXML(ref3, account_id1, symbol1, amount1, limit1, trans_id1);
    // transactionXML(ref4, account_id2, symbol2, amount2, limit2, trans_id2);
    // std::stringstream ss_1;
    // ss_1<<ref.length()<<'\n'<<ref;
    // ref = ss_1.str();
    // std::stringstream ss_2;
    // ss_2<<ref2.length()<<'\n'<<ref2;
    // ref2 = ss_2.str();
    // std::stringstream ss_3;
    // ss_3<<ref3.length()<<'\n'<<ref3;
    // ref3 = ss_3.str();
    // std::stringstream ss_4;
    // ss_4<<ref4.length()<<'\n'<<ref4;
    // ref4 = ss_4.str();


    

    // std::vector<std::string> requests;
    // requests.push_back(ref);
    // requests.push_back(ref2);
    // requests.push_back(ref3);
    // requests.push_back(ref4);
    
    // for(int i = 0; i < requests.size(); i++){
    // // while(1){
    //     //int len = trySelect(client_fd);
        
    //     // if(len == 0){
    //     //     break;
    //     // }
    //     // std::cout<<"counter: "<< counter<< " send test from client: "<<ref.c_str() << "len: "<<ref.length() +1 <<std::endl;
    //     //std::cout<<"counter: "<< counter<< " send test from client: "<<requests[i].c_str() << "len: "<<requests[i].length() +1 <<std::endl;
    //     std::cout <<"send the " <<i<< "th test: "<< requests[i] <<"length is:"<< requests[i].length() + 1<< std::endl;
    //     send(client_fd, requests[i].c_str(), requests[i].length() + 1, 0);
    //     //int send_len = send(client_fd, ref.c_str(), ref.length() + 1, 0);
        
    //     //send(client_fd, ref2.c_str(), ref2.length() + 1, 0);
        
    // //}
    // }
}


    // ref = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" \
    // "<create><account id=\"1234567890\" balance=\"1000\"/>" \
    // "symbol sym=\"SPY\">" \
    // "<account id=\"1234567890\">100000</acount>" \
    // "</symbol>" \
    // "</create>";




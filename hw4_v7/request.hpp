#include <stdio.h>
#include <string.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <sstream>

class Createmsg{
public:
    std::string create_msg;
    std::vector<Account> accounts;
    std::vector<Position> positions;

    //constructor
    Createmsg(std::string input){
        if(input.find("<create>") != std::string::npos){
            create_msg = input;
        }
        
    }
};

class Account{
public:
    std::string id;
    double balance;
};

class Position{
public:
    std::string account_id;
    std::string symbol;
    double amount;
};

class Transaction{
public:
    std::string trans_info;
    std::vector<Order*> orders;
    std::vector<Query*> queries;
    std::vector<Cancel*> cancels;
}

class Order{
public:
    std::string order_info;
    std::string symbol;
    double amount;
    double price;
    std::string status;
};



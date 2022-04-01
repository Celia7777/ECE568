#ifndef DB_PROCESS_HPP
#define DB_PROCESS_HPP

#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
// #include <mutex>
// extern std::mutex account_mtx;
// extern std::mutex position_mtx;
// extern std::mutex open_mtx;
// extern std::mutex executed_mtx;
// extern std::mutex cancel_mtx;

using namespace pqxx;

//open order for status and canceled response
class open_order{
public:
    double shares;
};
//canceled order for status and canceled response
class canceled_order{
public:
    double shares;
    long time;
};
//executed order for status and canceled response
class executed_order{
public:
    double shares;
    double price;
    long time;
};

//drop existing table
void dropTable(connection* C);
//create new table
void createTable(connection* C);
//load info into table
//<create>
void load_single_account(connection* C, std::string account_id, double balance);
int load_single_position(connection* C, std::string account_id, std::string symbol_name, double amount);
//<transactions>
std::string load_single_order(connection* C, std::string account_id, std::string symbol_name, double amount, double limit);
void MatchOrder(connection* C, std::string account_id, int order_id, std::string symbol, double amount, double price);
//request: query, response: status
void query_order(connection* C, std::string account_id, int order_id, std::vector<open_order>& open, std::vector<canceled_order>& cancel, std::vector<executed_order>& executed);
//request: cancel, response: canceled
void cancel_order(connection* C, std::string account_id, int order_id, std::vector<canceled_order>& cancel, std::vector<executed_order>& executed);



#endif
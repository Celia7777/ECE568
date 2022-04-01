#ifndef XML_PROCESS_HPP
#define XML_PROCESS_HPP

#include "tinyxml2.h"
#include <string>
#include <vector>

using namespace tinyxml2;

// request
void createXML(std::string& ref, std::string id, std::string balance, std::string sym, int symNum);
void transactionXML(std::string& ref, std::string id, std::string sym, std::string amt, std::string lmt, std::string trans_id);

//response
void res_create(std::string& ref, std::vector<std::pair<int, int>> flag, std::vector<std::string>& id, std::vector<std::pair<std::string,std::string>>& symbol);
// void res_transaction(std::string& ref, std::vector<std::pair<int, int>> flag, std::vector<std::string>& order_sym, std::vector<std::string>& order_amt, std::vector<std::string>& order_lmt, std::vector<std::string>& query_id, std::vector<std::string>& cancel_id );
void res_transaction(std::string& ref, std::vector<std::pair<int, int>> flag);

// std::string testxml = "<?xml version="1.0" encoding="UTF-8"?>\n<create>\n<account id="1234567890" balance="1000"/>\n</create>";

#endif


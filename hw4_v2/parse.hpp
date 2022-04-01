#include <stdio.h>
#include <string.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <sstream>
#include <mutex>

#include "db_process.hpp"
#include "tinyxml2.h"
using namespace tinyxml2;
using namespace pqxx;

extern std::mutex account_mtx;
extern std::mutex position_mtx;
extern std::mutex open_mtx;
extern std::mutex executed_mtx;
extern std::mutex cancel_mtx;


void ParseRequets(connection*C, std::string input);
void ParseCreateinfo(XMLElement* root, connection*C);
void ParseTransinfo(XMLElement* root, connection*C);
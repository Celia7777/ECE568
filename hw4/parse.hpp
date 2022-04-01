#include <stdio.h>
#include <string.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <sstream>

#include "db_process.hpp"
#include "tinyxml2.h"
using namespace tinyxml2;
using namespace pqxx;

void ParseCreateinfo( connection* C, std::string input);
void ParseTransinfo(connection* C, std::string input);
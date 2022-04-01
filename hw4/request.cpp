
// Order::Order(std::string input){
//     if(input.find("</transactions>") != std::string::npos){
//         order_info = input;
//     }
// }

//method functions for <create>
//parse accounts



//parse id and balance
void Account::ParseAccinfo(){
    //parse id
    std::size_t f_id = account_info.find("id");
    if(f_id != std::string::npos){
        std::size_t f_quot1 = account_info.find('"', f_id + 4);
        if(f_quot1 != std::string::npos){
            id = account_info.substr(f_id + 4, f_quot1 - f_id - 4);
            //parse balance
            std::size_t f_balance = account_info.find("balance=", f_quot1 + 2);
            if(f_balance != std::string::npos){
                std::size_t f_quot2 = account_info.find('"', f_balance + 9);
                std::string balance_str = account_info.substr(f_balance + 9, f_quot2 - f_balance - 9);
                balance = std::stod(balance_str);
            }
        }
    }

}

//parse symbols
void Account::ParseSymbol(){
    std::size_t id_len = id.length();
    std::size_t f_sym = account_info.find("symbol sym=");
    while(f_sym != std::string::npos){
        std::size_t f_quot = account_info.find('"', f_sym + 12);
        if(f_quot != std::string::npos){
            std::string sym_str = account_info.substr(f_sym + 12, f_quot - f_sym - 12);
            std::size_t f_id = account_info.find("account id=", f_quot);
            if(f_id != std::string::npos){
                std::size_t f_rbrack = account_info.find('>', f_id);
                if(f_rbrack != std::string::npos){
                    std::size_t f_lbrack = account_info.find('<', f_id + 11 + id_len + 3);
                    std::string amount_str = account_info.substr(f_rbrack + 1, f_lbrack - f_rbrack - 1);
                    double amount = std::stod(amount_str);
                    positions.push_back(sym_str, amount);
                }
            }
        }
        f_sym = account_info.find("symbol sym=", f_lbrack);
    }
}


//method functions for <transactions>
void Order::
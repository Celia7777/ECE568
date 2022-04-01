#include "db_process.hpp"
// using namespace pqxx;



// drop existing table
void dropTable(connection* C){
    //drop account table
    std::string account = "DROP TABLE IF EXISTS ACCOUNT CASCADE;";
    work W(*C);
    W.exec(account);
    // W_account.commit();
    //drop position table
    std::string position = "DROP TABLE IF EXISTS POSITION CASCADE;";
    // work W_position(*C);
    W.exec(position);
    // W_position.commit();
    //drop transactions table
    std::string open = "DROP TABLE IF EXISTS OPEN CASCADE;";
    // work W_transactions(*C);
    W.exec(open);
    // W_transactions.commit();
    //drop order table
    std::string executed = "DROP TABLE IF EXISTS EXECUTED CASCADE;";
    // work W_order(*C);
    W.exec(executed);
    std::string cancel = "DROP TABLE IF EXISTS CANCEL CASCADE;";
    W.exec(cancel);
    W.commit();
}


//create new table
void createTable(connection* C){
    std::string content, line;
    std::string fileName = "table_creation.txt";
    std::ifstream readFile(fileName.c_str());
    if(readFile.is_open()){
        while (getline(readFile, line)){
            content += line;
        }
        readFile.close();
    }
    else{
        std::cerr << "Could not open txt file to create tables!" << std::endl;
    }
    work W(*C);
    W.exec(content);
    W.commit();
    return;
}

//load one account info into ACCOUNT table
void load_single_account(connection* C, std::string account_id, double balance){
    //create account table
    //check whether account_id exits
    // std::stringstream find_id;
    // find_id << "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID=" <<"\'" <<account_id<<"\'" << ";";
    // nontransaction N(*C);
    // result R(N.exec(find_id.str()));
    // auto it = R.begin();
    // //if not, insert new account into table
    // if (it == R.end()){
    //     std::stringstream insert_account;
    //     insert_account << "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE) VALUES (" << account_id << ", " << balance << ");";
    //     work W(*C);
    //     W.exec(insert_account.str());
    //     W.commit();
    // }
    work W(*C);
    std::stringstream find_id;
    find_id << "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID=" << W.quote(account_id) << ";";
    result R(W.exec(find_id.str()));
    auto it = R.begin();
    //if not, insert new account into table
    if (it == R.end()){
        std::stringstream insert_account;
        insert_account << "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE) VALUES (" << W.quote(account_id) << ", " << balance << ");";
        W.exec(insert_account.str());
        W.commit();
    }
    // std::stringstream insert_account;
    // insert_account << "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE) VALUES (" << account_id << ", " << balance << ");";
    // work W(*C);
    // W.exec(insert_account.str());
    // W.commit();
}

//load one position info into POSITION table
int load_single_position(connection* C, std::string account_id, std::string symbol_name, double amount){
    //check whether that ACCOUBT_ID exits
    work W(*C);
    std::stringstream find_id;
    find_id << "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID=" << W.quote(account_id) << ";";
    result R(W.exec(find_id.str()));
    auto it = R.begin();
    //if there is no that ACCOUNT_ID, return false
    if (it == R.end()){
        // exit(EXIT_FAILURE);
        return 1;
    }
    //if there is that ACCOUNT_ID
    //check whether that symbol exits
    std::stringstream find_symbol;
    find_symbol << "SELECT AMOUNT FROM POSITION WHERE ACCOUNT_ID=" << W.quote(account_id) << " AND SYMBOL=" << W.quote(symbol_name) <<";";
    result R_sym(W.exec(find_symbol.str()));
    auto it_sym = R_sym.begin();
    //if there is no that symbol, add new position to table
    if (it_sym == R_sym.end()){
        std::stringstream insert_sym;
        insert_sym << "INSERT INTO POSITION (ACCOUNT_ID, SYMBOL, AMOUNT) VALUES (" << W.quote(account_id) << ", " << W.quote(symbol_name) << ", " << amount << ");";
        W.exec(insert_sym.str());
        W.commit();
        // exit(EXIT_SUCCESS);
        return 0;
    }
    //if there is that symbol, update amount info to table
    std::stringstream add_amount;
    add_amount << "UPDATE POSITION SET AMOUNT=" << (amount + it_sym[0].as<double>()) <<" WHERE ACCOUNT_ID=" << W.quote(account_id) << " AND SYMBOL=" << W.quote(symbol_name) << ";";
    W.exec(add_amount.str());
    W.commit();
    // exit(EXIT_SUCCESS);
    return 0;
}

//load one order info into OPEN table
std::string load_single_order(connection* C, std::string account_id, std::string symbol_name, double amount, double limit){
    //check whether that ACCOUBT_ID exits
    work W(*C);
    std::stringstream find_acc_id;
    find_acc_id << "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID=" << W.quote(account_id) << ";";
    result R(W.exec(find_acc_id.str()));
    auto it = R.begin();
    //if there is no that ACCOUNT_ID, return false
    if (it == R.end()){
        exit(EXIT_FAILURE);
    }
    //if there is that ACCOUNT_ID
    time_t current_time = time(NULL);
    std::stringstream insert_order;
    insert_order << "INSERT INTO OPEN (ACCOUNT_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES (" << W.quote(account_id) << ", " << W.quote(symbol_name) << ", " << amount << ", " << limit << ", " << current_time << ");";
    W.exec(insert_order.str());
    
    //return order_id (convert int to string)
    std::stringstream find_order_id;
    // use time to identify one specific order id
    find_order_id << "SELECT ORDER_ID FROM OPEN WHERE ACCOUNT_ID = " << W.quote(account_id) << " AND TIME=" << current_time << ";";
    result R_order_id(W.exec(find_order_id.str()));
    int order_id = -1;
    auto it_order_id = R_order_id.begin();
    while (it_order_id != R_order_id.end()){
        order_id = it_order_id[0].as<int>();
        it_order_id++;
    }    
    W.commit();
    return std::to_string(order_id);
}

//match the current order with the open table
// CREATE TABLE OPEN(
// ORDER_ID SERIAL,
// ACCOUNT_ID VARCHAR(256),
// SYMBOL VARCHAR(256),
// AMOUNT DOUBLE PRECISION,
// LIMIT DOUBLE PRECISION,
// TIME BIGINT,
// PRIMARY KEY (ORDER_ID),
// FOREIGN KEY (ACCOUNT_ID) REFERENCES ACCOUNT (ACCOUNT_ID) ON DELETE SET NULL ON UPDATE CASCADE
// );

void MatchOrder(connection* C, std::string account_id, int order_id, std::string symbol, double amount, double price){
    work W(*C);
    std::stringstream find_order;
    //process seller order
    if(amount < 0){
        find_order << "SELECT ORDER_ID, ACCOUNT_ID, SYMBOL, AMOUNT, PRICE, TIME FROM OPEN WHERE AMOUNT>0 AND SYMBOL=" << "\'"<<symbol<<"\'"<<" AND ACCOUNT_ID <>"<<W.quote(account_id) <<"AND PRICE>"<<price << " ORDER BY TIME ASC"<< ";";
        result Order(W.exec(find_order.str()));
        for(auto it = Order.begin(); it != Order.end(); ++it){
            //exactly match
            if (abs(it[3].as<double>()) == abs(amount)){
                //delete buy order
                std::stringstream delete_open_buy;
                delete_open_buy << "DELETE FROM OPEN WHERE ORDER_ID="<<it[0].as<int>()<<";";
                W.exec(delete_open_buy.str());
                //delete sell order
                std::stringstream delete_open_sell;
                delete_open_sell << "DELETE FROM OPEN WHERE ORDER_ID="<<order_id<<";";
                W.exec(delete_open_sell.str());
                //insert buy into EXECUTED table, buy info is from select
                std::stringstream insert_executed_buy;
                insert_executed_buy << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(it[1].as<std::string>())<<", "<<it[0].as<int>()<<", "<<W.quote(it[2].as<std::string>())<<", "<<it[3].as<double>()<<", "<<it[4].as<double>()<<", " << it[5].as<long>() << ");";
                W.exec(insert_executed_buy.str());
                //insert sell into EXECUTED table, sell info is from parameters
                std::stringstream insert_executed_sell;
                insert_executed_sell << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(account_id)<<", "<<order_id<<", "<<W.quote(it[2].as<std::string>())<<", "<<amount<<", "<<it[4].as<double>()<<", " << it[5].as<long>() << ");";
                W.exec(insert_executed_sell.str());
            }
            //buy order shares < sell amount
            else if(abs(it[3].as<double>()) < abs(amount)){
                time_t current_time = time(NULL);
                //delete buy order
                std::stringstream delete_open_buy;
                delete_open_buy << "DELETE FROM OPEN WHERE ORDER_ID="<<it[0].as<int>()<<";";
                W.exec(delete_open_buy.str());
                //insert buy into EXECUTED table, buy info is from select
                std::stringstream insert_executed_buy;
                insert_executed_buy << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(it[1].as<std::string>())<<", "<<it[0].as<int>()<<", "<<W.quote(it[2].as<std::string>())<<", "<<it[3].as<double>()<<", "<<it[4].as<double>()<<", "<<it[5].as<long>() << ");";
                W.exec(insert_executed_buy.str());
                //insert sell into EXECUTED table, sell info is from parameters
                std::stringstream insert_executed_sell;
                insert_executed_sell << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(account_id)<<", "<<order_id<<", "<<W.quote(it[2].as<std::string>())<<", "<< (-it[3].as<double>())<<", "<<it[4].as<double>()<<", "<<it[5].as<long>() << ");";
                W.exec(insert_executed_sell.str());
                //update left sell amount in OPEN table 
                std::stringstream insert_open_sell;
                insert_open_sell << "UPDATE OPEN SET AMOUNT=" << (it[3].as<double>() + amount) << " WHERE ORDER_ID=" << order_id << ";";
                W.exec(insert_open_sell.str());
            }
            //buy order share > sell amount
            else{
                //delete sell order
                std::stringstream delete_open_sell;
                delete_open_sell << "DELETE FROM OPEN WHERE ORDER_ID="<<order_id<<";";
                W.exec(delete_open_sell.str());
                //update buy info in OPEN table, buy info is from select
                std::stringstream update_open_buy;
                update_open_buy << "UPDATE OPEN SET AMOUNT=" << (it[3].as<double>() + amount) << " WHERE ORDER_ID=" << it[0].as<int>() << ";";
                W.exec(update_open_buy.str());
                //insert sell into EXECUTED table, sell info is from parameters
                std::stringstream insert_executed_sell;
                insert_executed_sell << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(account_id)<<", "<<order_id<<", "<<W.quote(it[2].as<std::string>())<<", "<< amount <<", "<<it[4].as<double>()<<", " << it[5].as<long>() << ");";
                W.exec(insert_executed_sell.str());
            }
        }
    }
    //process buyer order
    //amount > 0
    else{
        find_order << "SELECT ORDER_ID, ACCOUNT_ID, SYMBOL, AMOUNT, PRICE, TIME FROM OPEN WHERE AMOUNT<0 AND SYMBOL=" << "\'"<<symbol<<"\'"<<" AND ACCOUNT_ID <>"<<W.quote(account_id) <<"AND PRICE<"<<price << " ORDER BY TIME ASC"<< ";";
        result Order(W.exec(find_order.str()));
        for(auto it = Order.begin(); it != Order.end(); it++){
            //exactly match
            if (abs(it[3].as<double>()) == abs(amount)){
                //delete buy order
                std::stringstream delete_open_sell;
                delete_open_sell << "DELETE FROM OPEN WHERE ORDER_ID="<<it[0].as<int>()<<";";
                W.exec(delete_open_sell.str());
                //delete sell order
                std::stringstream delete_open_buy;
                delete_open_buy << "DELETE FROM OPEN WHERE ORDER_ID="<<order_id<<";";
                W.exec(delete_open_buy.str());
                //insert sell into EXECUTED table, sell info is from select
                std::stringstream insert_executed_sell;
                insert_executed_sell << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(it[1].as<std::string>())<<", "<<it[0].as<int>()<<", "<<W.quote(it[2].as<std::string>())<<", "<<it[3].as<double>()<<", "<<it[4].as<double>()<<", " << it[5].as<long>() << ");";
                W.exec(insert_executed_sell.str());
                //insert buy into EXECUTED table, buy info is from parameters
                std::stringstream insert_executed_buy;
                insert_executed_buy << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(account_id)<<", "<<order_id<<", "<<W.quote(it[2].as<std::string>())<<", "<<amount<<", "<<it[4].as<double>()<<", " << it[5].as<long>() << ");";
                W.exec(insert_executed_buy.str());
            }
            //buy order shares < sell amount
            else if(abs(it[3].as<double>()) < abs(amount)){
                time_t current_time = time(NULL);
                //delete buy order
                std::stringstream delete_open_buy;
                delete_open_buy << "DELETE FROM OPEN WHERE ORDER_ID="<<order_id<<";";
                W.exec(delete_open_buy.str());
                //insert buy into EXECUTED table, buy info is from parameter
                std::stringstream insert_executed_buy;
                insert_executed_buy << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(account_id)<<", "<<order_id<<", "<<W.quote(symbol)<<", "<<amount<<", "<<price<<", "<<time << ");";
                W.exec(insert_executed_buy.str());
                //insert sell into EXECUTED table, sell info is from select
                std::stringstream insert_executed_sell;
                insert_executed_sell << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(it[1].as<std::string>())<<", "<<it[0].as<int>()<<", "<<W.quote(it[2].as<std::string>())<<", "<< -amount<<", "<<price<<", "<< time<< ");";
                W.exec(insert_executed_sell.str());
                //update left sell amount in OPEN table 
                std::stringstream insert_open_sell;
                insert_open_sell << "UPDATE OPEN SET AMOUNT=" << (it[3].as<double>() + amount) << " WHERE ORDER_ID=" << order_id << ";";
                W.exec(insert_open_sell.str());
            }
            //buy order share > sell amount
            else{
                //delete sell order
                std::stringstream delete_open_sell;
                delete_open_sell << "DELETE FROM OPEN WHERE ORDER_ID="<<it[0].as<int>()<<";";
                W.exec(delete_open_sell.str());
                //update buy info in OPEN table, buy info is from select
                std::stringstream update_open_buy;
                update_open_buy << "UPDATE OPEN SET AMOUNT=" << (it[3].as<double>() + amount) << " WHERE ORDER_ID=" << order_id << ";";
                W.exec(update_open_buy.str());
                //insert sell into EXECUTED table, sell info is from parameters
                std::stringstream insert_executed_sell;
                insert_executed_sell << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(it[1].as<std::string>())<<", "<<it[0].as<int>()<<", "<<W.quote(it[2].as<std::string>())<<", "<< -amount <<", "<<price<<", " << time << ");";
                W.exec(insert_executed_sell.str());
            }
        }
    }

    W.commit();
}




//request: query, response: status
void query_order(connection* C, std::string account_id, int order_id, std::vector<open_order>& open, std::vector<canceled_order>& cancel, std::vector<executed_order>& executed){
    //check whether that ORDER_ID exits in OPEN table
    std::stringstream find_open;
    find_open << "SELECT AMOUNT FROM OPEN WHERE ORDER_ID=" << order_id  << ";";
    nontransaction N(*C);
    result R_open(N.exec(find_open.str()));
    auto it_open = R_open.begin();
    if (it_open != R_open.end()){
        open_order open_temp;
        for (it_open = R_open.begin(); it_open != R_open.end(); it_open++){
            //push back executed order info
            open_temp.shares = it_open[0].as<double>();
            open.push_back(open_temp);
        }
    }
    //check whether that ORDER_ID exits in EXECUTED table
    std::stringstream find_executed;
    find_executed << "SELECT AMOUNT, PRICE, TIME FROM EXECUTED WHERE ORDER_ID=" << order_id << ";";
    result R_executed(N.exec(find_executed.str()));
    auto it_executed = R_executed.begin();
    if (it_executed != R_executed.end()){
        executed_order executed_temp;
        for (it_executed = R_executed.begin(); it_executed != R_executed.end(); it_executed++){
            //push back executed order info
            executed_temp.shares = it_executed[0].as<double>();
            executed_temp.price = it_executed[1].as<double>();
            executed_temp.time = it_executed[2].as<long>();
            executed.push_back(executed_temp);
        }
    }
    //check whether that ORDER_ID exits in CANCEL table
    std::stringstream find_cancel;
    find_cancel << "SELECT AMOUNT, TIME FROM CANCEL WHERE ORDER_ID=" << order_id << ";";
    result R_cancel(N.exec(find_cancel.str()));
    auto it_cancel = R_cancel.begin();
    if (it_cancel != R_cancel.end()){
        canceled_order cancel_temp;
        for (it_cancel = R_cancel.begin(); it_cancel != R_cancel.end(); it_cancel++){
            //push back executed order info
            cancel_temp.shares = it_cancel[0].as<double>();
            cancel_temp.time = it_cancel[1].as<long>();
            cancel.push_back(cancel_temp);
        }
    }
}

//request: cancel, response: canceled
void cancel_order(connection* C, std::string account_id, int order_id, std::vector<canceled_order>& cancel, std::vector<executed_order>& executed){
    //check whether that ORDER_ID exits in OPEN table
    std::stringstream find_order_id;
    find_order_id << "SELECT AMOUNT FROM OPEN WHERE ORDER_ID=" << order_id << ";";
    work W(*C);
    result R(W.exec(find_order_id.str()));
    auto it = R.begin();
    //if there is that ORDER_ID, change that open order into cancel order
    if (it != R.end()){
        canceled_order cancel_temp;
        for ( it = R.begin(); it != R.end(); it++){
            //push back canceled order info
            cancel_temp.shares = it[0].as<double>();
            time_t current_time = time(NULL);
            cancel_temp.time = current_time;
            cancel.push_back(cancel_temp);
            //insert canceled order info into CANCEL tabel 
            std::stringstream insert_cancel;
            insert_cancel << "INSERT INTO CANCEL (ACCOUNT_ID, TRANS_ID, AMOUNT, TIME) VALUES (" << "\'" << account_id << "\', " << order_id << ", " << it[0].as<int>() << ", " << current_time << ");";
            W.exec(insert_cancel.str());
            // W.commit();
            //delete open order info in OPEN table
            std::stringstream delete_open;
            delete_open << "DELETE FROM OPEN WHERE ORDER_ID=" << order_id << ";";
            W.exec(delete_open.str());
            // W.commit();
        }
    }
    //check whetheer that ORDER_ID exits in EXECUTED table
    std::stringstream find_id_executed;
    find_id_executed << "SELECT AMOUNT, PRICE, TIME FROM EXECUTED WHERE ORDER_ID=" << order_id << ";";
    result R_executed(W.exec(find_id_executed.str()));
    auto it_executed = R_executed.begin();
    if (it_executed != R_executed.end()){
        executed_order executed_temp;
        for (it_executed = R_executed.begin(); it_executed != R_executed.end(); it_executed++){
            //push back executed order info
            executed_temp.shares = it_executed[0].as<double>();
            executed_temp.price = it_executed[1].as<double>();
            executed_temp.time = it_executed[2].as<long>();
            executed.push_back(executed_temp);
        }
    }
    W.commit();
}





// work W_account(*C);
// work W_position(*C);
// std::stringstream content;
// std::vector<std::pair<std::string, double>>::iteration it;
// int position_id = 0;
// for (it = accountInfo.begin(); it != accountInfo.end(); it ++){
//     content_account << "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE, POSITION_ID) VALUES" << " (" << accountInfo.id << ", " << accountInfo.balance << ", " << position_id << ");";
//     // need to check whether that account id exits or not
//     content_position << "INSERT INTO POSITION (POSITION_ID, ACCOUNT_ID, SYMBOL, AMOUNT) VALUES" << " (" << position_id << " ," << accountInfo.id << " ," << it.first << ", " << it.second << ");";
//     position_id ++;
//     W_account.exec(content_account.str());
//     W_account.commit();
//     W_position.exec(content_position.str());
//     W_position.commit();
// }



// void MatchOrder(connection* C, std::string account_id, int order_id, std::string symbol, double amount, double price){
//     work W(*C);
//     std::stringstream find_order;
//     //process seller order
//     if(amount < 0){
//         find_order << "SELECT ORDER_ID, ACCOUNT_ID, SYMBOL, AMOUNT, PRICE, TIME FROM OPEN WHERE AMOUNT>0 AND SYMBOL=" << "\'"<<symbol<<"\'"<<" AND ACCOUNT_ID <>"<<W.quote(account_id) <<"AND PRICE>"<<price << " ORDER BY TIME ASC"<< ";";
//     }
//     //process buyer order
//     else{
//         find_order << "SELECT ORDER_ID, ACCOUNT_ID, SYMBOL, AMOUNT, PRICE, TIME FROM OPEN WHERE AMOUNT<0 AND SYMBOL=" << "\'"<<symbol<<"\'"<<" AND ACCOUNT_ID <>"<<W.quote(account_id) <<"AND PRICE<"<<price << " ORDER BY TIME ASC"<< ";";
//     }
//     result Order(W.exec(find_order.str()));
//     //if found matched orders
//     //execute transactions, and update tables
//     for(auto it = Order.begin(); it != Order.end(); ++it){
//         //exactly match
//         if(abs(it[3].as<double>()) == abs(amount)){
//             std::stringstream delete_open;
//             delete_open << "DELETE FROM OPEN WHERE ORDER_ID="<<it[0].as<int>()<<";";
//             W.exec(delete_open.str());
//             // W.commit();
//             //insert executed
//             std::stringstream insert_executed_buy;
//             std::stringstream insert_executed_sell;
//             insert_executed_buy << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(it[1].as<std::string>())<<", "<<it[0].as<int>()<<", "<<W.quote(it[2].as<std::string>())<<", "<<it[3].as<double>()<<", "<<it[4].as<double>()<<", " << it[5].as<double>() << ";";
//             insert_executed_sell << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(it[1].as<std::string>())<<", "<<it[0].as<int>()<<", "<<W.quote(it[2].as<std::string>())<<", "<<amount<<", "<<it[4].as<double>()<<", "<<it[5].as<double>() << ";";
//             W.exec(insert_executed_buy.str());
//             W.exec(insert_executed_sell.str());
//             // W.commit();
//             amount = 0;
//         }
//         //if sell or buy order shares < needed amount
//         //delete that sell or buy order and insert buy and sell order into EXECUTED table
//         else if(abs(it[3].as<double>()) < abs(amount)){
//             std::stringstream delete_open;
//             delete_open << "DELETE FROM OPEN WHERE ORDER_ID="<<it[0].as<int>()<<";";
//             W.exec(delete_open.str());
//             W.commit();
//             //insert executed
//             std::stringstream insert_executed_buy;
//             std::stringstream insert_executed_sell;
//             insert_executed_buy << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(it[1].as<std::string>())<<", "<<it[0].as<int>()<<", "<<W.quote(it[2].as<std::string>())<<", "<<it[3].as<double>()<<", "<<it[4].as<double>()<<", "<<it[5].as<double>() << ";";
//             insert_executed_sell << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(it[1].as<std::string>())<<", "<< order_id <<", "<<W.quote(it[2].as<std::string>())<<", "<<(-it[3].as<double>())<<", "<<it[4].as<double>()<<", "<<it[5].as<double>() << ";";
//             W.exec(insert_executed_buy.str());
//             W.exec(insert_executed_sell.str());
//             // W.commit();
//             if(amount < 0){
//                 amount = amount + it[3].as<double>();
//             }
//             else{
//                 amount = amount - abs(it[3].as<double>());
//             }

//         }
//         //if sell or buy order shares > needed amount
//         //insert sell or buy order into EXECUTED table
//         else{
//             //update open
//             std::stringstream update_open;
//             update_open << "UPDATE OPEN SET AMOUNT=" << (it[3].as<double>() - amount) << " WHERE ORDER_ID=" << it[0].as<int>() << ";";
//             W.exec(update_open.str());
//             // W.commit();
//             //insert executed
//             std::stringstream insert_executed_buy;
//             std::stringstream insert_executed_sell;
//             insert_executed_buy << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(it[1].as<std::string>())<<", "<<it[0].as<int>()<<", "<<W.quote(it[2].as<std::string>())<<", "<<(-amount)<<", "<<it[4].as<double>()<<", "<<it[5].as<double>() << ";";
//             insert_executed_sell << "INSERT INTO EXECUTED (ACCOUNT_ID, ORDER_ID, SYMBOL, AMOUNT, PRICE, TIME) VALUES("<<W.quote(it[1].as<std::string>())<<", "<<it[0].as<int>()<<", "<<W.quote(it[2].as<std::string>())<<", "<<amount<<", "<<it[4].as<double>()<<", "<<it[5].as<double>() << ";";
//             W.exec(insert_executed_buy.str());
//             W.exec(insert_executed_sell.str());
//             // W.commit();
//             amount = 0;
//         }
//     }
//     if(abs(amount) > 0){
//         //update open
//         std::stringstream update_open;
//         update_open << "UPDATE OPEN SET AMOUNT="<<amount<<"WHERE ORDER_ID="<<order_id << ";";
//         W.exec(update_open.str());
//         // W.commit();
//     }
//     W.commit();
// }
#include "server.hpp"

//test print database content
void print_account(connection* C){
    std::stringstream temp;
    work W(*C);
    temp << "SELECT * FROM ACCOUNT;";
    W.commit();
    nontransaction N(*C);
    result R(N.exec(temp.str()));
    std::cout << "ACCOUNT_ID BALANCE" << std::endl;
    for (result::iterator it = R.begin(); it != R.end(); it++){
        std::cout << it[0].as<std::string>() << " " << it[1].as<double>() << std::endl;
    }
}
void print_position(connection* C){
    std::stringstream temp;
    work W(*C);
    temp << "SELECT * FROM POSITION;";
    W.commit();
    nontransaction N(*C);
    result R(N.exec(temp.str()));
    std::cout << "POSITION_ID ACCOUNT_ID SYMBOL AMOUNT" << std::endl;
    for (result::iterator it = R.begin(); it != R.end(); it++){
        std::cout << it[0].as<int>() << " " << it[1].as<std::string>() << " " << it[2].as<std::string>() << " " << it[3].as<double>() << std::endl;
    }
}
void print_open(connection* C){
    std::stringstream temp;
    work W(*C);
    temp << "SELECT * FROM OPEN;";
    W.commit();
    nontransaction N(*C);
    result R(N.exec(temp.str()));
    std::cout << "ORDER_ID ACCOUNT_ID SYMBOL AMOUNT PRICE TIME" << std::endl;
    for (result::iterator it = R.begin(); it != R.end(); it++){
        std::cout << it[0].as<int>() << " " << it[1].as<std::string>() << " " << it[2].as<std::string>() << " " << it[3].as<double>() << " " << it[4].as<double>() << " " << it[5].as<long>() << std::endl;
    }
}
void print_executed(connection* C){
    std::stringstream temp;
    work W(*C);
    temp << "SELECT * FROM EXECUTED;";
    W.commit();
    nontransaction N(*C);
    result R(N.exec(temp.str()));
    std::cout << "EXECUTED_ID ACCOUNT_ID ORDER_ID SYMBOL AMOUNT PRICE TIME" << std::endl;
    for (result::iterator it = R.begin(); it != R.end(); it++){
        std::cout << it[0].as<int>() << " " << it[1].as<std::string>() << " " << it[2].as<int>() << " " << it[3].as<std::string>() << " " << it[4].as<double>() << " " << it[5].as<double>() << " " <<it[6].as<long>() << std::endl;
    }
}

// CANCEL_ID SERIAL,
// ACCOUNT_ID VARCHAR(256),
// TRANS_ID INT,
// AMOUNT DOUBLE PRECISION,
// TIME BIGINT,
void print_cancel(connection* C){
    std::stringstream temp;
    work W(*C);
    temp << "SELECT * FROM EXECUTED;";
    W.commit();
    nontransaction N(*C);
    result R(N.exec(temp.str()));
    std::cout << "CANCEL_ID ACCOUNT_ID TRANS_ID SYMBOL AMOUNT TIME" << std::endl;
    for (result::iterator it = R.begin(); it != R.end(); it++){
        std::cout << it[0].as<int>() << " " << it[1].as<std::string>() << " " << it[2].as<int>() << " " << it[3].as<std::string>() << " " <<it[5].as<long>() << std::endl;
    }
}


int main(int argc, char* argv[])
{
    int server_socket = setup(PORT);
    std::string request;

    connection* C;
    // connect_db(C);
    //allocate & initialize a Postgres connection object
    try{
        C = new connection("dbname=matching_server user=postgres password=passw0rd");
        if(C->is_open()){
            std::cout << "Opened database successfully: " << C->dbname() << std::endl;
        }
        else{
            std::cout << "" << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return 1;
    }
    // dropTable(C);
    // createTable(C);

    while(1){
        std::string client_ip;
        int connect_to_client_fd = toAccept(server_socket, client_ip);

        char rqst[65536] = {0};
        int rqst_len = recv(connect_to_client_fd, rqst, sizeof(rqst), 0);
        if(rqst_len==-1){
            std::cout << "Error in receive first request from client" << std::endl;
            return EXIT_FAILURE;
        }
        request.assign(rqst, rqst_len);
        // std::cout<<"received length: "<<rqst_len<<std::endl;
        // std::cout << "the first request is:" << request <<std::endl;
        //if request is <create>
        if(request.find("create") != std::string::npos){
            ParseCreateinfo( C, request);
        }
        //if request if <transactions>
        if(request.find("transactions") != std::string::npos){
            ParseTransinfo(C, request);
        }
        std::cout << "============print begin================"<< std::endl;
        print_account(C);
        print_position(C);
        print_open(C);
        print_executed(C);
        print_cancel(C);
        std::cout << "============print end================"<< std::endl;
    }

    C->disconnect();
    return 0;
}  

    
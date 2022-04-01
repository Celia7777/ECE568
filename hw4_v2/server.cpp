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
    temp << "SELECT * FROM CANCEL;";
    W.commit();
    nontransaction N(*C);
    result R(N.exec(temp.str()));
    std::cout << "CANCEL_ID ACCOUNT_ID TRANS_ID AMOUNT TIME" << std::endl;
    for (result::iterator it = R.begin(); it != R.end(); it++){
        std::cout << it[0].as<int>() << " " << it[1].as<std::string>() << " " << it[2].as<int>() << " " << it[3].as<double>() << " " <<it[4].as<long>() << std::endl;
    }
}

int main(int argc, char* argv[])
{
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
    dropTable(C);
    createTable(C);
    C->disconnect();
    delete C;
    // dropTable(C);
    // createTable(C);

    int server_socket = setup(PORT);
    std::string client_ip;
    int connect_to_client_fd;
    while(1){
        connect_to_client_fd = toAccept(server_socket, client_ip);
        if(connect_to_client_fd == -1){
            std::cerr << "Error in connect to client socket" << std::endl;
            return EXIT_FAILURE;
        }
        //multithreads
        std::cout<<"start process requets in multi-threads"<<std::endl;
        std::thread newthread(ProcessClientrqst, connect_to_client_fd);
        newthread.detach();
    }

    //C->disconnect();
    close(server_socket);
    return 0;
}

void ProcessClientrqst(int connect_to_client_fd){
    std::cout<<"enter in request process function"<<std::endl;
    connection* C;
    //connect database
    try{
        C = new connection("dbname=matching_server user=postgres password=passw0rd");
        if(C->is_open()){
            std::cout << "Opened database successfully: " << C->dbname() << std::endl;
        }
        else{
            std::cout << "" << std::endl;
            return;
        }
    }
    catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return;
    }
    
    char rqst[65536] = {0};
    
    while(1){
        memset(rqst, 0, sizeof(rqst));
        int rqst_len = recv(connect_to_client_fd, rqst, sizeof(rqst), 0);
        if(rqst_len == 0){
            break;
        }
        if(rqst_len==-1){
            std::cerr << "Error in receive first request from client" << std::endl;
            return;
        }
        
        std::string request;
        request.assign(rqst, rqst_len);
    
        std::cout<<"received length: "<<rqst_len<<std::endl;
        std::cout << "the first request is:" << request <<std::endl;
        
        std::string request_p = request;
        //std::size_t f_num = request_p.find("\n");
        while(request_p.length() > 0){
            std::size_t f_num = request_p.find('\n');
            std::cout<<"find the first line: "<<f_num<<std::endl;
            std::string num_str = request_p.substr(0,f_num);
            std::cout<<"the num_str is:"<<num_str<<"*"<<std::endl;
            // std::stringstream ss;
            // ss<<num_str;
            // int num;
            // ss>>num;
            int num = std::stoi(num_str);
            std::cout<<"the num:"<<num<<std::endl;
            std::string process = request_p.substr(f_num + 1, num);
            ParseRequets(C, process);
            request_p = request_p.substr(f_num + num + 2, std::string::npos);
            std::cout<<"after num cut, rqst is:"<<request_p<<std::endl;
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
    delete C;
    
}



    
#include "server.hpp"

//ofstream dout("results.txt");


//test print database content
void print_account(connection* C){
    std::stringstream temp;
    work W(*C);
    temp << "SELECT * FROM ACCOUNT;";
    W.commit();
    nontransaction N(*C);
    result R(N.exec(temp.str()));
    std::cout << "ACCOUNT_ID BALANCE" << std::endl;
    //dout << "ACCOUNT_ID BALANCE" << std::endl;
    for (result::iterator it = R.begin(); it != R.end(); it++){
        std::cout << it[0].as<std::string>() << " " << it[1].as<double>() << std::endl;
        //dout << it[0].as<std::string>() << " " << it[1].as<double>() << std::endl;
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
    //dout << "POSITION_ID ACCOUNT_ID SYMBOL AMOUNT" << std::endl;
    for (result::iterator it = R.begin(); it != R.end(); it++){
        std::cout << it[0].as<int>() << " " << it[1].as<std::string>() << " " << it[2].as<std::string>() << " " << it[3].as<double>() << std::endl;
        //dout << it[0].as<int>() << " " << it[1].as<std::string>() << " " << it[2].as<std::string>() << " " << it[3].as<double>() << std::endl;
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
    //dout << "ORDER_ID ACCOUNT_ID SYMBOL AMOUNT PRICE TIME" << std::endl;
    for (result::iterator it = R.begin(); it != R.end(); it++){
        std::cout << it[0].as<int>() << " " << it[1].as<std::string>() << " " << it[2].as<std::string>() << " " << it[3].as<double>() << " " << it[4].as<double>() << " " << it[5].as<long>() << std::endl;
        //dout << it[0].as<int>() << " " << it[1].as<std::string>() << " " << it[2].as<std::string>() << " " << it[3].as<double>() << " " << it[4].as<double>() << " " << it[5].as<long>() << std::endl;
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

std::string receiveAllmsg(int fd, std::string msg, int& total){
    //std::cout << "message length is" << msg.length() << "chunk is: " << chunk << std::endl;
    // if(!chunk && msg.length()<=65536){
    //     total+=msg.length();
    //     return msg;
    // }
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
    //std::cout<<"enter in request process function"<<std::endl;
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
    int counter = 0;
    
    time_t start = time(NULL);
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
        //request = receiveAllmsg(connect_to_client_fd, request, rqst_len);
    
        std::cout<<"received length: "<<rqst_len<<std::endl;
        //std::cout << "the first request is:" << request <<std::endl;
        
        std::string request_p = request;
        std::string response;
        //std::size_t f_num = request_p.find("\n");
        while(request_p.length() > 0){
            //std::cout<<"in while loop for parsing"<<std::endl;
            std::size_t f_num = request_p.find('\n');
            //std::cout<<"find the first line: "<<f_num<<std::endl;
            std::string num_str = request_p.substr(0,f_num);
            //std::cout<<"the num_str is:"<<num_str<<"*"<<std::endl;
            // std::stringstream ss;
            // ss<<num_str;
            // int num;
            // ss>>num;
            int num = std::stoi(num_str);
            //std::cout<<"the num:"<<num<<std::endl;
            std::string process = request_p.substr(f_num + 1, num);
            //std::cout<<"start parsing requets"<<std::endl;
            ParseRequets(C, process, response);
            std::cout<<"response generated in server"<<response<<std::endl;
            int send_len = send(connect_to_client_fd, response.c_str(), response.length() + 1, 0);
            std::cout<<"send response length is: "<<send_len<<std::endl;
            counter++;
            std::cout<<"server counter: "<<counter<<std::endl;
            request_p = request_p.substr(f_num + num + 2, std::string::npos);
            //std::cout<<"after num cut, rqst is:"<<request_p<<std::endl;
        }
        // int send_len = send(connect_to_client_fd, response.c_str(), response.length() + 1, 0);
        // std::cout<<"send response length is: "<<send_len<<std::endl;

        // std::cout << "============print begin================"<< std::endl;
        // print_account(C);
        // print_position(C);
        // print_open(C);
        // print_executed(C);
        // print_cancel(C);
        // std::cout << "============print end================"<< std::endl;
        time_t end = time(NULL);
        std::cout<<"runtime is: "<<(end - start)<<std::endl;
    }
    C->disconnect();
    delete C;
    
    
}

    
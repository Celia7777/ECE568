#include "server.hpp"


int main(void){
    int server_socket = setup(PORT);
    std::string request;

    //initial database
    connection* C;
    connect_db(C);
    dropTable(C);
    createTable(C);

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
        else{
            ParseTransinfo(C, request);
        }
    }

    C->disconnect();
}  

    
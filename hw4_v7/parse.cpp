#include "parse.hpp"
// std::mutex account_mtx;
// std::mutex position_mtx;
// std::mutex open_mtx;
// std::mutex executed_mtx;
// std::mutex cancel_mtx;

//handle the clients' requests
void ParseRequets(connection*C, std::string input, std::string & resp){
    XMLDocument doc;
    doc.Parse(input.c_str());

    XMLElement* root = doc.RootElement();
        if(strcmp(root->Name(), "create") == 0){
            //std::cout<<"in <create>"<<std::endl;
            ParseCreateinfo(root, C, resp);
            //root = root->NextSiblingElement();
        }
        else if(strcmp(root->Name(), "transactions") == 0){
            //std::cout<<"in <transactions>"<<std::endl;
            ParseTransinfo(root, C, resp);
            //root = root->NextSiblingElement();
        }
}



//parse requests
void ParseCreateinfo(XMLElement* root, connection* C, std::string & ref){
    std::vector<int> id_sym;
    std::vector<int> status;
    std::vector<std::string> vec_acc_id;
    std::vector<std::string> vec_sym;
    std::vector<std::string> vec_sym_id;

    // XMLDocument doc;
    // doc.Parse(input.c_str());

    // XMLElement* root = doc.RootElement();

    //parse create
    //if(strcmp(root->Name(), "create") == 0){
        XMLElement* child = root->FirstChildElement();
        while(child){
            //account id & balance
            if(strcmp(child->Name(), "account") == 0){
                id_sym.push_back(0);
                //std::cout<<"account id is: "<<child->Attribute("id")<<" balance is: "<<child->Attribute("balance")<<std::endl;
                //account_mtx.lock();
                int temp = load_single_account(C, child->Attribute("id"), std::atof(child->Attribute("balance")));
                vec_acc_id.push_back(child->Attribute("id"));
                if (temp == 0){
                    status.push_back(0);
                }
                else{
                    status.push_back(1);
                }
                //account_mtx.unlock();
            }
            //positions: symbol + amount
            else if(strcmp(child->Name(), "symbol") == 0){
                id_sym.push_back(1);
                //std::cout<<"position: symbol name is: "<<child->Attribute("sym")<<std::endl;
                XMLElement* sym_child = child->FirstChildElement();
                while(sym_child){
                    if(strcmp(sym_child->Name(), "account") == 0){
                        //std::cout<<"position: account id is: "<<sym_child->Attribute("id")<<"symbol amount is: "<<sym_child->GetText()<<std::endl;
                        // position_mtx.lock();
                        // account_mtx.lock();
                        int temp = load_single_position(C, sym_child->Attribute("id"), child->Attribute("sym") , std::atof(sym_child->GetText()));
                        vec_sym_id.push_back(sym_child->Attribute("id"));
                        vec_sym.push_back(child->Attribute("sym"));
                        if (temp == 0){
                            status.push_back(0);
                        }
                        else{
                            status.push_back(1);
                        }
                        // double test_num = 1000;
                        // std::cout<<"position: account id is: "<<sym_child->Attribute("id")<<" symbol amount is: "<<test_num<<std::endl;
                        // load_single_position(C, sym_child->Attribute("id"), child->Attribute("sym") , test_num);
                        // position_mtx.unlock();
                        // account_mtx.unlock();
                    }
                    sym_child = sym_child->NextSiblingElement();
                }
            }
            else{
                std::cerr<<"error format of create XML"<<std::endl;
            }
            child = child->NextSiblingElement();
            
        }
    //}
    response_createXML(ref, id_sym, status , vec_acc_id, vec_sym, vec_sym_id);
}


void ParseTransinfo(XMLElement* root, connection*C, std::string &ref){
    // XMLDocument doc;
    // doc.Parse(input.c_str());

    // XMLElement* root = doc.RootElement();

    //parse transanctions
    //if(strcmp(root->Name(), "transactions") == 0){
        //::cout<<"in trans if"<<std::endl;
        //account id
        //std::cout<<"account id in this transaction is: "<<root->Attribute("id")<<std::endl;
        XMLElement* child = root->FirstChildElement();
        while(child){
            //order
            if(strcmp(child->Name(), "order") == 0){
                //std::cout<<"in this order: the symbol is: "<<child->Attribute("sym")<<", the amount is: "<<child->Attribute("amount")<<", the limit price is: "<<child->Attribute("limit")<<std::endl;
                // open_mtx.lock();
                // account_mtx.lock();
                std::string order_id = load_single_order(C, root->Attribute("id"), child->Attribute("sym"), std::atof(child->Attribute("amount")), std::atof(child->Attribute("limit")));
                // open_mtx.unlock();
                // account_mtx.unlock();

                // open_mtx.lock();
                // executed_mtx.lock();
                MatchOrder(C, root->Attribute("id"), std::atoi(order_id.c_str()), child->Attribute("sym"), std::atof(child->Attribute("amount")), std::atof(child->Attribute("limit")));
                // open_mtx.unlock();
                // executed_mtx.unlock();
            }
            //query
            else if(strcmp(child->Name(), "query") == 0){
                //std::cout<<"the trans id(order id) for query is: "<<child->Attribute("id")<<std::endl;
                std::vector<open_order> open;
                std::vector<canceled_order> cancel;
                std::vector<executed_order> executed;
                // open_mtx.lock();
                // executed_mtx.lock();
                // cancel_mtx.lock();
                query_order(C, root->Attribute("id"), std::atoi(child->Attribute("id")), open, cancel, executed);
                // open_mtx.unlock();
                // executed_mtx.unlock();
                // cancel_mtx.unlock();
            }
            //cancel
            else if(strcmp(child->Name(), "cancel") == 0){
                //std::cout<<"the trans id(order id) for cancel is: "<<child->Attribute("id")<<std::endl;
                std::vector<canceled_order> cancel;
                std::vector<executed_order> executed;
                // open_mtx.lock();
                // executed_mtx.lock();
                // cancel_mtx.lock();
                cancel_order(C, root->Attribute("id"), std::atoi(child->Attribute("id")), cancel, executed);
                // open_mtx.unlock();
                // executed_mtx.unlock();
                // cancel_mtx.unlock();
            }
            else{
                std::cerr<<"error format of create XML"<<std::endl;
            }
            child = child->NextSiblingElement();
        }
    //}
    
}

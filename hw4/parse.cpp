#include "parse.hpp"

//parse requests
void ParseCreateinfo(connection*C, std::string input){
    XMLDocument doc;
    doc.Parse(input.c_str());

    XMLElement* root = doc.RootElement();

    //parse create
    if(strcmp(root->Name(), "create") == 0){
        XMLElement* child = root->FirstChildElement();
        while(child){
            //account id & balance
            if(strcmp(child->Name(), "account") == 0){
                std::cout<<"account id is: "<<child->Attribute("id")<<"balance is: "<<child->Attribute("balance")<<std::endl;
                load_single_account(C, child->Attribute("id"), std::atof(child->Attribute("balance")));
            }
            //positions: symbol + amount
            else if(strcmp(child->Name(), "symbol") == 0){
                std::cout<<"position: symbol name is: "<<child->Attribute("sym")<<std::endl;
                XMLElement* sym_child = child->FirstChildElement();
                while(sym_child){
                    if(strcmp(sym_child->Name(), "account") == 0){
                        std::cout<<"position: account id is: "<<sym_child->Attribute("id")<<"symbol amount is: "<<sym_child->GetText()<<std::endl;
                        load_single_position(C, sym_child->Attribute("id"), child->Attribute("sym") ,std::atof(sym_child->GetText()));
                    }
                    sym_child = sym_child->NextSiblingElement();
                }
            }
            else{
                std::cerr<<"error format of create XML"<<std::endl;
            }
            child = child->NextSiblingElement();
            
        }
    }
}


void ParseTransinfo(connection*C, std::string input){
    XMLDocument doc;
    doc.Parse(input.c_str());

    XMLElement* root = doc.RootElement();

    //parse transanctions
    if(strcmp(root->Name(), "transactions") == 0){
        //account id
        std::cout<<"account id in this transaction is: "<<root->Attribute("id")<<std::endl;
        XMLElement* child = root->FirstChildElement();
        while(child){
            //order
            if(strcmp(child->Name(), "order") == 0){
                std::cout<<"in this order: the symbol is: "<<child->Attribute("sym")<<", the amount is: "<<child->Attribute("amount")<<", the limit price is: "<<child->Attribute("limit")<<std::endl;
                std::string order_id = load_single_order(C, root->Attribute("id"), child->Attribute("sym"), std::atof(child->Attribute("amount")), std::atof(child->Attribute("limit")));
                MatchOrder(C, root->Attribute("id"), std::atoi(order_id.c_str()), child->Attribute("sym"), std::atof(child->Attribute("amount")), std::atof(child->Attribute("limit")));
            }
            //query
            else if(strcmp(child->Name(), "query") == 0){
                std::cout<<"the trans id(order id) for query is: "<<child->Attribute("id")<<std::endl;
                std::vector<open_order> open;
                std::vector<canceled_order> cancel;
                std::vector<executed_order> executed;
                query_order(C, root->Attribute("id"), std::atoi(child->Attribute("id")), open, cancel, executed);
            }
            //cancel
            else if(strcmp(child->Name(), "cancel") == 0){
                std::cout<<"the trans id(order id) for cancel is: "<<child->Attribute("id")<<std::endl;
                std::vector<canceled_order> cancel;
                std::vector<executed_order> executed;
                cancel_order(C, root->Attribute("id"), std::atoi(child->Attribute("id")), cancel, executed);
            }
            else{
                std::cerr<<"error format of create XML"<<std::endl;
            }
            child = child->NextSiblingElement();
        }
    }
}

#include "xml_process.hpp"

//generate XML format for <create>
void createXML(std::string& ref,std::string id, std::string balance, std::string sym, int symNum){
    //declaration
    const char* declaration = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    //create
    XMLDocument doc;
    doc.Parse(declaration);
    //add root
    XMLElement* root = doc.NewElement("create");
    //add account
    XMLElement* account = doc.NewElement("account");
    account->SetAttribute("id", id.c_str());
    account->SetAttribute("balance", balance.c_str());
    // //add account2
    // XMLElement* account2 = doc.NewElement("account");
    // std::string id2 = "88888";
    // std::string balance2 = "666666";
    // account2->SetAttribute("id", id2.c_str());
    // account2->SetAttribute("balance", balance2.c_str());
    //add symbol
    XMLElement* symbol = doc.NewElement("symbol");
    symbol->SetAttribute("sym", sym.c_str());
    XMLElement* symbol_account = doc.NewElement("account");
    symbol_account->SetAttribute("id",id.c_str());
    XMLText* symbol_num = doc.NewText(std::to_string(symNum).c_str());
    symbol->InsertEndChild(symbol_account);
    symbol_account->InsertEndChild(symbol_num);
    // //add symbol2
    // XMLElement* symbol2 = doc.NewElement("symbol");
    // std::string sym2 = "TIRED";
    // symbol2->SetAttribute("sym", sym2.c_str());
    // XMLElement* symbol_account2 = doc.NewElement("account");
    // symbol_account2->SetAttribute("id",id.c_str());
    // int symNum2 = 5555;
    // XMLText* symol_num2 = doc.NewText(std::to_string(symNum2).c_str());
    // symbol2->InsertEndChild(symbol_account2);
    // symbol_account2->InsertEndChild(symol_num2);
    //insert end child
    doc.InsertEndChild(root);
    root->InsertEndChild(account);
    // root->InsertEndChild(account2);
    root->InsertEndChild(symbol);
    // root->InsertEndChild(symbol2);
    //convert to string
    XMLPrinter printer;
    doc.Print(&printer);
    ref = printer.CStr();
    // doc.Print();
}
//generate XML format for <transaction>
void transactionXML(std::string& ref, std::string id, std::string sym, std::string amt, std::string lmt, std::string trans_id, int mode){
    XMLDocument doc;
    //add root
    XMLElement* root = doc.NewElement("transactions");
    root->SetAttribute("id", id.c_str());
    //add order
    XMLElement* order = doc.NewElement("order");
    order->SetAttribute("sym", sym.c_str());
    order->SetAttribute("amount", amt.c_str());
    order->SetAttribute("limit", lmt.c_str());
    // //add query
    XMLElement* query = doc.NewElement("query");
    query->SetAttribute("id", trans_id.c_str());
    // //add cancel
    XMLElement* cancel = doc.NewElement("cancel");
    cancel->SetAttribute("id", trans_id.c_str());
    //insert end child
    doc.InsertEndChild(root);
    if (mode == 0){
        root->InsertEndChild(order);
    }
    else if (mode == 1){
        root->InsertEndChild(query);
    }
    else{
        root->InsertEndChild(cancel);
    }
    // root->InsertEndChild(order);
    // root->InsertEndChild(query);
    // root->InsertEndChild(cancel);
    // convert to string
    XMLPrinter printer;
    doc.Print(&printer);
    ref = printer.CStr();
    // doc.Print();
}

void response_createXML(std::string& ref, std::vector<int>& id_sym, std::vector<int>& status , std::vector<std::string>& id, std::vector<std::string>& symbol, std::vector<std::string>& sym_id){
    XMLDocument doc;
    XMLElement* root = doc.NewElement("results");
    int it_id = 0;
    int it_sym = 0;
    for(int i =0; i< id_sym.size(); i++){
        //<create> account
        if (id_sym[i] == 0){
            //success
            if (status[i] == 0){
                XMLElement* success_id = doc.NewElement("created");
                success_id->SetAttribute("id", id[it_id].c_str());
                root->InsertEndChild(success_id);
            }
            //fail
            else{
                XMLElement* fail_id = doc.NewElement("error");
                fail_id->SetAttribute("id", id[it_id].c_str());
                XMLText* msg = doc.NewText("Create Account ID Fail!");
                // account->InsertEndChild();
                fail_id->InsertEndChild(msg);
                root->InsertEndChild(fail_id);
            }
            it_id++;
        }
        //<create> symbol
        else{
            //success
            if (status[i] == 0){
                XMLElement* success_sym = doc.NewElement("created");
                success_sym->SetAttribute("sym", symbol[it_sym].c_str());
                success_sym->SetAttribute("id", sym_id[it_sym].c_str());
                root->InsertEndChild(success_sym);
            }
            //fail
            else{
                XMLElement* fail_sym = doc.NewElement("error");
                fail_sym->SetAttribute("sym", symbol[it_sym].c_str());
                fail_sym->SetAttribute("id", sym_id[it_sym].c_str());
                XMLText* msg = doc.NewText("Create Symbol Fail!");
                fail_sym->InsertEndChild(msg);
                root->InsertEndChild(fail_sym);
            }
            it_sym++;
        }
    }
    doc.InsertEndChild(root);
    XMLPrinter printer;
    doc.Print(&printer);
    ref = printer.CStr();
}
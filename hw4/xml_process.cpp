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
    //add symbol
    XMLElement* symbol = doc.NewElement("symbol");
    symbol->SetAttribute("sym", sym.c_str());
    XMLElement* symbol_account = doc.NewElement("account");
    symbol_account->SetAttribute("id",id.c_str());
    //insert end child
    symbol->InsertEndChild(symbol_account);
    doc.InsertEndChild(root);
    root->InsertEndChild(account);
    root->InsertEndChild(symbol);
    //convert to string
    XMLPrinter printer;
    doc.Print(&printer);
    ref = printer.CStr();
    // doc.Print();
}
//generate XML format for <transaction>
void transactionXML(std::string& ref, std::string id, std::string sym, std::string amt, std::string lmt, std::string trans_id){
    XMLDocument doc;
    //add root
    XMLElement* root = doc.NewElement("transactions");
    root->SetAttribute("id", id.c_str());
    //add order
    XMLElement* order = doc.NewElement("order");
    order->SetAttribute("sym", sym.c_str());
    order->SetAttribute("amount", amt.c_str());
    order->SetAttribute("limit", lmt.c_str());
    //add query
    XMLElement* query = doc.NewElement("query");
    query->SetAttribute("id", trans_id.c_str());
    //add cancel
    XMLElement* cancel = doc.NewElement("cancel");
    cancel->SetAttribute("id", trans_id.c_str());
    //insert end child
    doc.InsertEndChild(root);
    root->InsertEndChild(order);
    root->InsertEndChild(query);
    root->InsertEndChild(cancel);
    //convert to string
    XMLPrinter printer;
    doc.Print(&printer);
    ref = printer.CStr();
    // doc.Print();
}
//generate XML format for <results> of <create>
//std::pair first: 0:id/ 1:sym second: 0:success/1:fail
// void res_create(std::string& ref, std::vector<std::pair<int, int>> flag, std::vector<std::string>& id, std::vector<std::pair<std::string,std::string>>& symbol){
//     XMLDocument doc;
//     //add root
//     XMLElement* root = doc.NewElement("results");
//     //add children
//     std::vector<std::pair<int,int>>::iterator it;
//     int counter_id = 0;
//     int counter_sym = 0;
//     for (it = flag.begin(); it != flag.end(); it++ ){
//         //id
//         if (it->first == 0){
//             //success
//             if (it->second == 0){
//                 XMLElement* id_success = doc.NewElement("created");
//                 id_success->SetAttribute("id", id[counter_id].c_str());
//                 root->InsertEndChild(id_success);
//                 counter_id++;
//             }
//             //fail
//             if (it->second == 1){
//                 XMLElement* id_fail = doc.NewElement("error");
//                 id_fail->SetAttribute("id", id[counter_id].c_str());
//                 root->InsertEndChild(id_fail);
//                 counter_id++;
//             }
//         }
//         //sym
//         if (it->first == 1){
//             if (it->second == 0){
//                 XMLElement* sym_success = doc.NewElement("created");
//                 sym_success->SetAttribute("sym", (symbol[counter_sym].first).c_str());
//                 sym_success->SetAttribute("id", (symbol[counter_sym].second).c_str());
//                 root->InsertEndChild(sym_success);
//                 counter_sym++;
//             }
//             if (it->second == 1){
//                 XMLElement* sym_fail = doc.NewElement("error");
//                 sym_fail->SetAttribute("sym", (symbol[counter_sym].first).c_str());
//                 sym_fail->SetAttribute("id", (symbol[counter_sym].second).c_str());
//                 root->InsertEndChild(sym_fail);
//                 counter_sym++;
//             }
//         }
//     }
//     doc.InsertEndChild(root);
//     //contert to string
//     XMLPrinter printer;
//     doc.Print(&printer);
//     ref = printer.CStr();
// }

//generate XML format for <results> of <transactions>















    // StringWriter stringWriter = new StringWriter();
    // XmlTextWriter xmlTextWriter = new XmlTextWriter(stringWriter);
    // doc.WriteTo(xmlTextWriter);
    // ref = stringWriter.ToString();
    // ref = std::to_string(doc);
    // doc.SaveFile();
     // ref = doc.ToStr();

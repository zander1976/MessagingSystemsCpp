#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <string>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "messages.h"
#include "accounts.h"

class EditorMessageHandler : public MessageHandler {

public:
    EditorMessageHandler(key_t sendKey, key_t receiveKey) : MessageHandler(sendKey, receiveKey) {}

    void UpdateDatabase() {
        std::cout << "UpdateDatabase: "  << std::endl;
        sendMessage(UPDATE_DATABASE_REQUEST, "");
        receiveMessage();
    }

    virtual void onUpdateDatabaseComplete() {
        std::cout << "onUpdateDatabaseComplete: " << std::endl;
    }
};


int main() {

    // Create the message queue
    key_t serverKey = ftok("./server.cpp", 49);
    if (serverKey == -1) {
        std::perror("Server Key error");
        exit(1);
    }
    key_t editorkey = ftok("./editor.cpp", 51);
    if (editorkey == -1) {
        std::perror("Editor Key error");
        exit(1);
    }
    EditorMessageHandler editor(serverKey, editorkey);

    std::regex accountPattern("\\d{5}");
    std::regex pinPattern("\\d{3}");    
    std::regex fundsPattern("\\d+\\.\\d{2}");

    while(true) {
        std::string account;
        std::string password;
        std::string funds;

        std::cout << "---------------------------------" << std::endl;
        std::cout << "Please enter your account number: ";
        std::cin >> account;
        if (account == "x") {
            std::cout << "Quit!" << std::endl;
            break;
        }
        if (!std::regex_match(account, accountPattern)) {
            std::cout << "Invalid account number format." << std::endl;
            continue;
        }            

        std::cout << "Please enter your password: ";
        std::cin >> password;
        if (!std::regex_match(password, pinPattern)) {
            std::cout << "Invalid account pin format." << std::endl;
            continue;
        }        

        std::cout << "Funds: ";
        std::cin >> funds;
        if (!std::regex_match(funds, fundsPattern)) {
            std::cout << "Invalid funds format." << std::endl;
            continue;
        }
        
        std::map<std::string, Account> accountList;
        accountList = deserializeAccounts("accounts.txt");

        // Look for existing account
        auto it = accountList.find(account);
        if (it != accountList.end()) {
            Account& bankrecord = it->second;
            bankrecord.accountNumber = account;
            bankrecord.accountPin = password;
            bankrecord.funds = funds;
        } else {
            Account newrecord;
            newrecord.accountNumber = account;
            newrecord.accountPin = password;
            newrecord.funds = funds;
            accountList[account] = newrecord;
        }
        
        serializeAccounts(accountList, "accounts.txt");
        
        editor.UpdateDatabase();
    }

    return 0;
}

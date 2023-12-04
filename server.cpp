#include <iostream>
#include <sstream>
#include <cstring>
#include <map>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "messages.h"
#include "accounts.h"

class ServerMessageHandler : public MessageHandler {
private: 
    bool running = true;
    int loginAttempts = 0;
    std::string loggedInAccount; 
    std::map<std::string, Account> accountList;

public:
    ServerMessageHandler(key_t sendKey, key_t receiveKey) : MessageHandler(sendKey, receiveKey) {}

    ~ServerMessageHandler() {
        std::cout << "MessageHandler destructor:" << std::endl;
        // Delete the server queue
        if (msgctl(senderId, IPC_RMID, nullptr) == -1) {
            std::perror("Error deleting server queue.");
        }

        // Delete the client queue
        if (msgctl(receiveId, IPC_RMID, nullptr) == -1) {
            std::perror("Error deleting client queue.");
        }
    } 
    
    void WaitForMessage() {
        std::cout << "WaitForMessage" << std::endl;
        while(running) {
            receiveMessage();
        }
    }
    
    // Override methods for specific message types
    void onLoginRequest(std::string message) override {
        std::cout << "onLoginRequest: " << message << std::endl;
        std::istringstream ss(message);
        std::string account;
        std::string pin;
        std::getline(ss, account, ',');
        std::getline(ss, pin, '\0');

        // The drawing showed that everything talks directly to the database
        accountList = deserializeAccounts("accounts.txt");

        // Look for existing account
        auto it = accountList.find(account);
        if (it != accountList.end()) {
            if (pin == it->second.accountPin) {
                loginAttempts = 0;
                loggedInAccount = account;
                sendMessage(LOGIN_SUCCESS, "");
                return;
            }
        }
        loginAttempts++;
        if (loginAttempts >= 3) {
            sendMessage(LOGIN_LOCKED, "");
            return;
        }
        
        sendMessage(LOGIN_PIN_WRONG, "");
    }

    virtual void onBalanceRequest() override {
        std::cout << "onBalanceRequest" << std::endl;
        accountList = deserializeAccounts("accounts.txt");
        auto it = accountList.find(loggedInAccount);
        if (it != accountList.end()) {
            sendMessage(BALANCE_FUNDS, it->second.funds);
            return;
        } 
        sendMessage(BALANCE_FUNDS, "0.00");
    }

    virtual void onWithdrawRequest() override {
        std::cout << "onWithdrawRequest" << std::endl;
        sendMessage(WITHDRAW_FUNDS_OK, "1000000");
        //sendMessage(WITHDRAW_NSF, "");
    }

    virtual void onQuit() override {
        std::cout << "onQuit" << std::endl;
        running=false;
    }

};

int main() {
    // Create the message queue
    key_t serverKey = ftok("./server.cpp", 49);
    if (serverKey == -1) {
        std::perror("Server Key error");
        exit(1);
    }
    key_t clientkey = ftok("./client.cpp", 50);
    if (clientkey == -1) {
        std::perror("Client Key error");
        exit(1);
    }

    ServerMessageHandler server(clientkey, serverKey);

    server.WaitForMessage();
    
    return 0;
}

#include <iostream>
#include <sstream>
#include <cstring>
#include <map>
#include <iomanip>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#ifndef __SERVER_H__
#define __SERVER_H__

int convertToCents(const std::string& str) {
    double value;
    std::istringstream iss(str);
    iss >> value;
    int cents = static_cast<int>(value * 100); 
    return cents;
}

std::string centsToDollars(int cents) {
    double dollars = static_cast<double>(cents) / 100.0;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << dollars;
    return oss.str();
}


class ServerMessageHandler : public MessageHandler {
private: 
    int editorId;
    bool running = true;
    int loginAttempts = 0;
    std::string loggedInAccount; 
    std::map<std::string, Account> accountList;

public:
    ServerMessageHandler(key_t sendKey, key_t receiveKey, key_t editorKey) : MessageHandler(sendKey, receiveKey) {
        editorId = msgget(editorKey, 0666 | IPC_CREAT);
        if (editorId == -1) {
            std::perror("Error creating server queue");
        }
        clearOldMessage(editorId);        
    }

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

        // Delete the client queue
        if (msgctl(editorId, IPC_RMID, nullptr) == -1) {
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
        sendMessage(ERROR, "Account not found!");
    }

    virtual void onWithdrawRequest(std::string message) override {
        std::cout << "onWithdrawRequest: " << message << std::endl;
        accountList = deserializeAccounts("accounts.txt");
        auto it = accountList.find(loggedInAccount);
        if (it != accountList.end()) {
            int balance = convertToCents(it->second.funds);
            int withdrawalAmount = convertToCents(message);

            int resultInCents = balance - withdrawalAmount; 
            if (resultInCents < 0) {
                sendMessage(WITHDRAW_NSF, "");
            }
            it->second.funds = centsToDollars(resultInCents);
            serializeAccounts(accountList, "accounts.txt");
            sendMessage(WITHDRAW_FUNDS_OK, message);
            return;
        } 
        sendMessage(ERROR, "Account not found!");
    }

    virtual void onUpdateDatabaseRequest() {
        std::cout << "onUpdateDatabase: " << std::endl;
        sendMessage(UPDATE_DATABASE_RESPONSE, "", editorId);
    }

    virtual void onQuit() override {
        std::cout << "onQuit" << std::endl;
        running=false;
    }
};


#endif // __SERVER_H__
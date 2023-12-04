#include <iostream>
#include <cstring>
#include <regex>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "messages.h"

class ClientMessageHandler : public MessageHandler {
public:
    ClientMessageHandler(key_t sendKey, key_t receiveKey) : MessageHandler(sendKey, receiveKey) {

    }

    void LoginRequest(std::string account, std::string pin) {
        std::cout << "LoginRequest: " << account + "," + pin << std::endl;
        sendMessage(LOGIN_REQUEST, account + "," + pin);
        receiveMessage();
    }

    void QuitRequest() {
        std::cout << "QuitRequest: " << std::endl;
        sendMessage(QUIT, "");
    }

    virtual void onLoginPinWrong(std::string message) {
        std::cout << "onLoginPinWrong: " << std::endl;
    }

    virtual void onLoginLocked(std::string message) {
        std::cout << "onLoginLocked: " << std::endl;
    }

    virtual void onLoginSuccess(std::string message) {
        std::cout << "onLoginSuccess: " << std::endl;
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

    ClientMessageHandler client(serverKey, clientkey);
    std::regex accountPattern("\\d{5}");
    std::regex pinPattern("\\d{3}");    
    std::regex fundsPattern("\\d+\\.\\d{2}");

    while (true) {
        std::string account;
        std::string pin;

        std::cout << "Please enter your account number: ";
        std::cin >> account;

        if (account == "x") {
            client.QuitRequest();
            std::cout << "Quit!" << std::endl;
            break;
        }
        if (!std::regex_match(account, accountPattern)) {
            std::cout << "Invalid account number format." << std::endl;
            continue;
        }            

        std::cout << "Please enter your password: ";
        std::cin >> pin;
        if (!std::regex_match(pin, pinPattern)) {
            std::cout << "Invalid account pin format." << std::endl;
            continue;
        } 
        client.LoginRequest(account, pin);


        // while(true) {
        //     // Logged in
        //     std::string choice; 
        //     std::string amount; 
        //     std::cout << "Menu (balance or withdrawal): ";
        //     std::cin >> choice;

        //     GenericMessage menuMessage;

        //     if (choice == "balance") {
        //         menuMessage.msgType = BALANCE_REQUEST;

        //     } else if (choice == "withdrawal") {
        //         menuMessage.msgType = WITHDRAW_REQUEST;
        //         std::cout << "Menu (balance or withdraw): ";
        //         std::cin >> amount;

        //         // Copy the string to message.msgResult with null-termination
        //         std::strncpy(message.msgResult, amount.c_str(), sizeof(message.msgResult) - 1);
        //         message.msgResult[sizeof(message.msgResult) - 1] = '\0'; // Ensure null-termination

        //         // Ensure the rest of the buffer is null-terminated if needed
        //         if (amount.length() < sizeof(message.msgResult)) {
        //             std::memset(message.msgResult + amount.length(), '\0', sizeof(message.msgResult) - amount.length());
        //         }                
        //     } else {
        //         continue;
        //     }
        //     if (msgsnd(serverid, &menuMessage, sizeof(menuMessage) - sizeof(long), 0) == -1) {
        //         std::perror("Send message failed");
        //         break;
        //     }

        //     GenericMessage response;
        //     msgrcv(clientid, &response, sizeof(response) - sizeof(long), 0, 0);

        //     std::cout << "Recieved: " << response.msgType << " - " << response.msgResult << std::endl;
        // }
    }

    return 0;
}

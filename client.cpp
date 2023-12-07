#include <iostream>
#include <cstring>
#include <regex>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "messages.h"
#include "client.h"


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

    std::regex accountPattern("\\d{5}");
    std::regex pinPattern("\\d{3}");    
    std::regex fundsPattern("\\d+\\.\\d{2}");

    while (true) {
        ClientMessageHandler client(serverKey, clientkey);

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

        if (client.isLocked()) {
            std::cout << "Account Locked" << std::endl;
            return 0;
        }

        if (!client.isLoggedIn()) {
            std::cout << "Login Failed" << std::endl;
            continue;
        }

         while(true) {
             // Logged in
             std::string choice; 
             std::string amount; 
             std::cout << "Menu (balance or withdrawal): ";
             std::cin >> choice;

            if (choice == "balance") {
                client.RequestBalance();
                break;
            } else if (choice == "withdrawal") {
                 std::cout << "Amount to withdral: ";
                 std::cin >> amount;
                client.ReuqestWithdrawal(amount);
                break;
            } else {
                 continue;
            }
        }
    }

    return 0;
}

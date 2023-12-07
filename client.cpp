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

#ifdef _WIN32
    system ("CLS");
#else
    system("clear");
#endif

    while (true) {

        std::cout << "\033[36mWelcome to Ben's Bank!\033[0m" << std::endl << std::endl;

        ClientMessageHandler client(serverKey, clientkey);

        std::string account;
        std::string pin;

        std::cout << "Please enter your account number (x to quit): ";
        std::cin >> account;

        if (account == "x") {
            client.QuitRequest();
            std::cout << "\033[32mThank you, Goodbye!\033[0m" << std::endl << std::endl;
            break;
        }
        if (!std::regex_match(account, accountPattern)) {
            std::cout << "\033[31mInvalid account number format.\033[0m" << std::endl << std::endl;
            continue;
        }            

        std::cout << "Please enter your password: ";
        std::cin >> pin;
        if (!std::regex_match(pin, pinPattern)) {
            std::cout << "\033[31mInvalid account pin format.\033[0m" << std::endl << std::endl;
            continue;
        } 
        client.LoginRequest(account, pin);

        if (client.isLocked()) {
            //std::cout << "\033[31mAccount Locked!\033[0m" << std::endl;
            std::cout << "\033[31mGoodbye!\033[0m" << std::endl << std::endl;
            client.QuitRequest();
            return 0;
        }

        if (!client.isLoggedIn()) {
            //std::cout << "Login Failed" << std::endl;
            continue;
        }

        while(true) {
             // Logged in
            std::string choice; 
            std::string amount; 
            std::cout << "\033[36mBen's Bank - Main Menu\033[0m" << std::endl << std::endl;

            std::cout << "Menu (balance or withdrawal or exit): ";
            std::cin >> choice;

            if (choice == "balance") {
                client.RequestBalance();
                break;
            } else if (choice == "withdrawal") {
                std::cout << "Amount to withdral: ";
                std::cin >> amount;
                if (!std::regex_match(amount, fundsPattern)) {
                    std::cout << "\033[31mInvalid fund format ($.$$)\033[0m" << std::endl << std::endl;
                    continue;
                } 
                client.ReuqestWithdrawal(amount);
                break;
            } else if (choice == "exit") {
                break;
            } else {
                continue;
            }
        }
        std::cout << "\033[32mThank you, Goodbye!\033[0m" << std::endl << std::endl;
    }

    return 0;
}

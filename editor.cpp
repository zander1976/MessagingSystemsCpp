#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <string>
#include <fcntl.h>
#include <semaphore.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "messages.h"
#include "accounts.h"
#include "editor.h"


int main() {
    // Create the semaphore
    sem_t *sem = sem_open("databasesem", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        std::perror("Semaphore creation/opening failed.");
        return 1;
    }    

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
    EditorMessageHandler editor(serverKey, editorkey, sem);

    std::regex accountPattern("\\d{5}");
    std::regex pinPattern("\\d{3}");    
    std::regex fundsPattern("\\d+\\.\\d{2}");
#ifdef _WIN32
    system ("CLS");
#else
    system("clear");
#endif

    while(true) {

        std::cout << "\033[36mWelcome to Ben's Bank Account Editor!\033[0m" << std::endl << std::endl;

        std::string account;
        std::string password;
        std::string funds;

        std::cout << "---------------------------------" << std::endl;
        std::cout << "Please enter your account number (x to quit): ";
        std::cin >> account;
        if (account == "x") {
            std::cout << "\033[32mThank you, Goodbye!\033[0m" << std::endl << std::endl;
            break;
        }
        if (!std::regex_match(account, accountPattern)) {
            std::cout << "\033[31mInvalid account number format.\033[0m" << std::endl << std::endl;
            continue;
        }            

        std::cout << "Please enter your password: ";
        std::cin >> password;
        if (!std::regex_match(password, pinPattern)) {
            std::cout << "\033[31mInvalid account pin format.\033[0m" << std::endl << std::endl;
            continue;
        }        

        std::cout << "Funds: ";
        std::cin >> funds;
        if (!std::regex_match(funds, fundsPattern)) {
            std::cout << "\033[31mInvalid fund format ($.$$)\033[0m" << std::endl << std::endl;
            continue;
        }
        
        std::map<std::string, Account> accountList;
        sem_wait(sem);
            accountList = deserializeAccounts("accounts.txt");
        sem_post(sem);

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
        
        sem_wait(sem);
            serializeAccounts(accountList, "accounts.txt");
        sem_post(sem);
        
        editor.UpdateDatabase();
    }

    return 0;
}

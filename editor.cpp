#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <regex>

struct Account {
    std::string accountNumber;
    std::string accountPin;
    std::string funds; // in cents
};

void serializeAccounts(const std::map<std::string, Account>& accountsMap, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file for serialization." << std::endl;
        return;
    }
    for (const auto& pair : accountsMap) {
        file << pair.second.accountNumber << "," << pair.second.accountPin << "," << pair.second.funds << "\n";
    }
    file.close();
    std::cout << "Map serialized to file: " << filename << std::endl;
}

std::map<std::string, Account> deserializeAccounts(const std::string& filename) {
    std::ifstream file(filename);
    std::map<std::string, Account> accountsMap;

    if (!file.is_open()) {
        std::cerr << "Unable to open file for deserialization." << std::endl;
        // Return an empty map or handle the error appropriately
        return accountsMap;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() == 3) {
            Account acc;
            acc.accountNumber = tokens[0];
            acc.accountPin = tokens[1];
            acc.funds = tokens[2];

            accountsMap[tokens[0]] = acc;
        }
    }
    file.close();

    return accountsMap;
}

int main() {
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
    }

    /*for (const auto& account : accountList) {
        std::cout << "Account Number: " << account.second.accountNumber << std::endl;
        std::cout << "Account Pin: " << account.second.accountPin << std::endl;
        std::cout << "Funds: " << account.second.funds << std::endl;
        std::cout << "----------------------" << std::endl;
    }

    auto it = accountList.find("00011");
    if (it != accountList.end()) {
        Account& desiredAccount = it->second;
        std::cout << "Found Account: " << desiredAccount.accountNumber << std::endl;
        std::cout << "Account Pin: " << desiredAccount.accountPin << std::endl;
        std::cout << "Funds: " << desiredAccount.funds << std::endl;
        desiredAccount.funds = "420.69";
        std::cout << "----------------------" << std::endl;
    } else {
        std::cout << "Account not found!" << std::endl;
    }

    for (const auto& account : accountList) {
        std::cout << "Account Number: " << account.second.accountNumber << std::endl;
        std::cout << "Account Pin: " << account.second.accountPin << std::endl;
        std::cout << "Funds: " << account.second.funds << std::endl;
        std::cout << "----------------------" << std::endl;
    }


    serializeAccounts(accountList, "accounts.txt");

    */
    return 0;
}

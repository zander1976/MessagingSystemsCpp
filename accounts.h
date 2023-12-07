#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <regex>

#ifndef __ACCOUNTS_H__
#define __ACCOUNTS_H__

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
    //std::cout << "Map serialized to file: " << filename << std::endl;
    std::cout << std::endl;
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


#endif 
#include <iostream>
#include <cstring>
#include <regex>

#ifndef __CLIENT_H__
#define __CLIENT_H__

class ClientMessageHandler : public MessageHandler {
private:
    bool locked = false;
    bool loggedIn = false;

public:
    ClientMessageHandler(key_t sendKey, key_t receiveKey) : MessageHandler(sendKey, receiveKey) {}

    void LoginRequest(std::string account, std::string pin) {
        sendMessage(LOGIN_REQUEST, account + "," + pin);
        receiveMessage();
    }

    void RequestBalance() {
        sendMessage(BALANCE_REQUEST, "");
        receiveMessage();
    }
    void ReuqestWithdrawal(std::string amount) {
        sendMessage(WITHDRAW_REQUEST, amount);
        receiveMessage();
    }

    void QuitRequest() {
        sendMessage(QUIT, "");
    }

    bool isLocked() {
        return locked;
    }

    bool isLoggedIn() {
        return loggedIn;
    }

    virtual void onLoginPinWrong() override {
        std::cout << "\033[31mIncorrect Pin. Please try again.\033[0m" << std::endl << std::endl;
    }

    virtual void onLoginLocked() override {
        std::cout << "\033[31mIncorrect Pin. Account is now Locked!\033[0m" << std::endl << std::endl;
        locked = true;
    }

    virtual void onLoginSuccess() override {
        std::cout << std::endl << "\033[32mLogin Sucessful!\033[0m" << std::endl << std::endl;
        loggedIn = true;
    }

    virtual void onBalanceFunds(std::string message)override {
        std::cout << "Your balance is: " << "\033[32m" << message << "\033[0m" << std::endl << std::endl;
    }

    virtual void onWithdrawFundsOK(std::string message) override {
        std::cout << "Successful Withdrawal amount: " << "\033[32m" << message << "\033[0m" << std::endl << std::endl;
    }

    virtual void onWithdrawNSF() override {
        std::cout << "\033[31mInsufficient funds\033[0m" << std::endl << std::endl;
    }

};


#endif // __CLIENT_H__
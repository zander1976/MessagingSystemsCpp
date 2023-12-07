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
        std::cout << "LoginRequest: " << account + "," + pin << std::endl;
        sendMessage(LOGIN_REQUEST, account + "," + pin);
        receiveMessage();
    }

    void RequestBalance() {
        std::cout << "RequestBalance: " << std::endl;
        sendMessage(BALANCE_REQUEST, "");
        receiveMessage();
    }
    void ReuqestWithdrawal(std::string amount) {
        std::cout << "ReuqestWithdrawal: " << amount << std::endl;
        sendMessage(WITHDRAW_REQUEST, amount);
        receiveMessage();
    }

    void QuitRequest() {
        std::cout << "QuitRequest: " << std::endl;
        sendMessage(QUIT, "");
    }

    bool isLocked() {
        return locked;
    }

    bool isLoggedIn() {
        return loggedIn;
    }

    virtual void onLoginPinWrong() override {
        std::cout << "onLoginPinWrong: " << std::endl;
    }

    virtual void onLoginLocked() override {
        std::cout << "onLoginLocked: " << std::endl;
        locked = true;
    }

    virtual void onLoginSuccess() override {
        std::cout << "onLoginSuccess: " << std::endl;
        loggedIn = true;
    }

    virtual void onBalanceFunds(std::string message)override {
        std::cout << "ReuqestWithdrawal: " << message << std::endl;
    }

    virtual void onWithdrawFundsOK(std::string message) override {
        std::cout << "ReuqestWithdrawal: " << message << std::endl;
    }

    virtual void onWithdrawNSF() override {
        std::cout << "ReuqestWithdrawal: " << std::endl;
    }

};


#endif // __CLIENT_H__
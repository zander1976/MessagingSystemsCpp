#include <iostream>
#include <sstream>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "messages.h"

class ServerMessageHandler : public MessageHandler {
public:
    ServerMessageHandler(key_t sendKey, key_t receiveKey) : MessageHandler(sendKey, receiveKey) {

    }
    
    // Override methods for specific message types
    void onLoginRequest(std::string message) override {
        std::cout << "onLoginRequest: " << message << std::endl;
        std::istringstream ss(message);
        std::string account;
        std::string pin;
        std::getline(ss, account, ',');
        std::getline(ss, pin, '\0');
        sendMessage(LOGIN_SUCCESS, "");
    }

    void WaitForMessage() {
        std::cout << "WaitForMessage" << std::endl;
        receiveMessage();
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

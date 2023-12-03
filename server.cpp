#include <iostream>
#include <sstream>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "messages.h"

int main() {
    // Create the message queue
    key_t serverKey = ftok("./server", 49);
    if (serverKey == -1) {
        std::perror("Server Key error");
        exit(1);
    }
    key_t clientkey = ftok("./client", 50);
    if (clientkey == -1) {
        std::perror("Client Key error");
        exit(1);
    }

    int serverid = msgget(serverKey, 0666 | IPC_CREAT);
    if (serverid == -1) {
        std::perror("Error creating server queue");
    }

    // Create the client queue
    int clientid = msgget(clientkey, 0666 | IPC_CREAT);
    if (clientid == -1) {
        std::perror("Error creating server queue");
    }

    // Clean out old server messages
    while (true) {
        GenericMessage tempMessage;
        // Try to receive a message
        ssize_t bytesReceived = msgrcv(serverid, &tempMessage, sizeof(tempMessage) - sizeof(long), 0, IPC_NOWAIT);

        if (bytesReceived == -1) {
            if (errno == ENOMSG) {
                // No more messages in the queue
                break;
            } else {
                // Handle other errors if needed
                std::perror("Error receiving message");
                break;
            }
        }
    }    

    // Clear out old client messages
    while (true) {

        GenericMessage tempMessage;
        // Try to receive a message
        ssize_t bytesReceived = msgrcv(clientid, &tempMessage, sizeof(tempMessage) - sizeof(long), 0, IPC_NOWAIT);

        if (bytesReceived == -1) {
            if (errno == ENOMSG) {
                // No more messages in the queue
                break;
            } else {
                // Handle other errors if needed
                std::perror("Error receiving message");
                break;
            }
        }
        // Discard received message
    }    
/*
    // Delete the server queue
    if ((msgctl(serverid, IPC_RMID, nullptr)) == -1) {
        std::perror("Error deleting server queue");
    }

    // Delete the client queue
    if ((msgctl(clientid, IPC_RMID, nullptr)) == -1) {
       std::perror("Error deleting client queue");
    }
     
    return 0;
*/
    while (true) {
        GenericMessage message;

        std::cout << "Waiting for messages:" << std::endl;
        if (msgrcv(serverid, &message, sizeof(message) - sizeof(long), 0, 0) == -1) {
            break;
        }

        std::cout << "Recieved: " << message.msgType << " - " << message.msgResult << std::endl;

        switch (message.msgType) {
            case LOGIN_REQUEST:
                std::cout << "LOGIN_REQUEST" << std::endl;
                std::string account;
                std::string password;
                std::string resultString(message.msgResult);
                std::istringstream ss(resultString);
                std::getline(ss, account, ',');
                std::getline(ss, password, '\0');

                // Using std::getline to split the string by comma
                std::cout << "Received Login Request: Account - " << account
                          << ", Password - " << password << std::endl;

                GenericMessage results;
                results.msgType = LOGIN_RESPONSE;
                if (account == "1234") {
                    strncpy(results.msgResult, "WRONG_PIN", sizeof(results.msgResult));
                } else if (account == "4321") {
                    strncpy(results.msgResult, "LOCKED", sizeof(results.msgResult));
                } else if (account == password) {
                    strncpy(results.msgResult, "LOGIN_OK", sizeof(results.msgResult));
                } else {
                    strncpy(results.msgResult, "FAILED", sizeof(results.msgResult));
                }
                if (msgsnd(clientid, &results, sizeof(results) - sizeof(long), 0) == -1) {
                    std::cerr << "Failed to send message: " << std::endl;
                    break;
                }
                break;
        }
   }

    // Delete the server queue
    if (msgctl(serverid, IPC_RMID, nullptr) == -1) {
        std::cerr << "Error deleting server queue." << std::endl;
    }

    // Delete the client queue
    if (msgctl(clientid, IPC_RMID, nullptr) == -1) {
        std::cerr << "Error deleting client queue." << std::endl;
    }    
    
    return 0;
}

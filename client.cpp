#include <iostream>
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
        std::string account;
        std::string password;

        std::cout << "Please enter your account number: ";
        std::cin >> account;

        if (account == "x") {
            message.msgType = QUIT;
            if (msgsnd(serverid, &message, sizeof(message) - sizeof(long), 0) == -1) {
                std::perror("Send message failed");
            }
            std::cout << "Quit!" << std::endl;
            break;
        }

        std::cout << "Please enter your password: ";
        std::cin >> password;

        message.msgType = LOGIN_REQUEST;
        
        std::strncpy(message.msgResult, account.c_str(), 5);
        message.msgResult[5] = '\0'; 
        std::strncat(message.msgResult, ",", 1);
        std::strncat(message.msgResult, password.c_str(), 3);
        message.msgResult[8] = '\0'; 
        std::cout << "Sending; " << message.msgResult << std::endl;
        std::memset(message.msgResult + 8, '\0', sizeof(message.msgResult) - 8);

        std::cout << "Sending Login Request: Account - " << account
                  << ", Password - " << password << std::endl;

        if (msgsnd(serverid, &message, sizeof(message) - sizeof(long), 0) == -1) {
            std::perror("Send message failed");
            break;
        }

        // Wait for the response to the login request
        GenericMessage response;
        msgrcv(clientid, &response, sizeof(response) - sizeof(long), 0, 0);

        std::cout << "Recieved: " << response.msgType << " - " << response.msgResult << std::endl;

        std::string resultString(response.msgResult);

        if (response.msgType == LOGIN_RESPONSE) {
            if (resultString == "WRONG_PIN") {
                std::cout << "Wrong Password" << std::endl;
                continue;
            } else if (resultString == "LOCKED") {
                std::cout << "Locked" << std::endl;
                break;
            } else if (resultString == "LOGIN_OK") {
                std::cout << "Login Success: " << std::endl;
                break;
            } else {
                std::cout << resultString << std::endl;
                std::perror("No idea how we got here!");
            }
        } else {
            std::perror("No idea how we got here!");
        }

        // Logged in
        std::string choice; 
        std::string amount; 
        std::cout << "Menu (balance or withdraw): ";
        std::cin >> choice;
        if (choice == "balance") {

        } else if (choice == "withdraw") {
            std::cout << "Menu (balance or withdraw): ";
            std::cin >> amount;
        }
    }

    // Delete the server queue
    if (msgctl(serverid, IPC_RMID, nullptr) == -1) {
        std::perror("Error deleting server queue.");
    }

    // Delete the client queue
    if (msgctl(clientid, IPC_RMID, nullptr) == -1) {
        std::perror("Error deleting client queue.");
    }

    return 0;
}

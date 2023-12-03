#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "messages.h"

int main() {
    // Create the message queue
    key_t serverKey = ftok("./serer", 49);
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

        //std::cout << "Received Login Request: Account - " << messageRecieved.account
        //          << ", Password - " << receivedLogin.password << std::endl;

        GenericMessage results;
        results.msgType = LOGIN_RESPONSE;
        strcpy(results.msgResult, "hello world");
/*
        if (strcmp(receivedLogin.account, "1234") == 0) {
            strncpy(results.msgResult, "WRONG_PIN", sizeof(results.msgResult));
        } else if (strcmp(receivedLogin.account, "4321") == 0) {
            strncpy(results.msgResult, "LOCKED", sizeof(results.msgResult));
        } else if (strcmp(receivedLogin.account, receivedLogin.password) == 0) {
            strncpy(results.msgResult, "LOGIN_OK", sizeof(results.msgResult));
        } else {
            // Handle other cases here
        }
*/
  
        if (msgsnd(clientid, &results, sizeof(results) - sizeof(long), 0) == -1) {
            std::cerr << "Failed to send message: " << std::endl;
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

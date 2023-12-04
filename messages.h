#include <iostream>
#include <string>
#include <sstream>

#ifndef __MESSAGES_H__
#define __MESSAGES_H__

enum MsgTypes {
    NONE,
    LOGIN_REQUEST,
    LOGIN_PIN_WRONG,
    LOGIN_LOCKED,
    LOGIN_SUCCESS,
    BALANCE_REQUEST,
    BALANCE_FUNDS,
    WITHDRAW_REQUEST,
    WITHDRAW_FUNDS_OK,
    WITHDRAW_NSF,
    UPDATE_DATABASE,
    QUIT,
};

struct GenericMessage {
    long msgType;
    char msgResult[100];
};


class MessageHandler {
private:
    int senderId;
    int receiveId;

protected:
    void clearOldMessage(int queueId) {
        // Clean out old server messages
        while (true) {
            GenericMessage tempMessage;
            // Try to receive a message
            ssize_t bytesReceived = msgrcv(queueId, &tempMessage, sizeof(tempMessage) - sizeof(long), 0, IPC_NOWAIT);

            if (bytesReceived == -1) {
                if (errno == ENOMSG) {
                    // No more messages in the queue
                    break;
                } else {
                    // Handle other errors if needed
                    std::perror("Error receiving message");
                }
            }
        }
    }

    int sendMessage(MsgTypes type, std::string message) {
        GenericMessage outgoingMessage;
        outgoingMessage.msgType = type;
        // Ensure null-termination by explicitly setting the last character to '\0'
        std::strncpy(outgoingMessage.msgResult, message.c_str(), sizeof(outgoingMessage.msgResult) - 1);
        outgoingMessage.msgResult[sizeof(outgoingMessage.msgResult) - 1] = '\0';

        if (msgsnd(senderId, &outgoingMessage, sizeof(outgoingMessage) - sizeof(long), 0) == -1) {
            std::perror("Send message failed");
            return -1;
        }
        return 0;
    }

    int receiveMessage() {
        GenericMessage incomingMessage;
        if (msgrcv(receiveId, &incomingMessage, sizeof(incomingMessage) - sizeof(long), 0, 0) == -1) {
            std::perror("Receive message failed");
            return -1;
        }
        std::string message(incomingMessage.msgResult);
        handleMessage(static_cast<MsgTypes>(incomingMessage.msgType), message);
        return 0;
    }

    virtual void handleMessage(MsgTypes type, std::string message) {
        switch (type) {
            case LOGIN_REQUEST:
                onLoginRequest(message);
                break;
            case LOGIN_PIN_WRONG:
                onLoginPinWrong(message);
                break;
            case LOGIN_LOCKED:
                onLoginLocked(message);
                break;
            case LOGIN_SUCCESS:
                onLoginSuccess(message);
                break;
            case BALANCE_REQUEST:
                onBalanceRequest();
                break;
            case BALANCE_FUNDS:
                onBalanceFunds();
                break;
            case WITHDRAW_REQUEST:
                onWithdrawRequest();
                break;
            case WITHDRAW_FUNDS_OK:
                onWithdrawFundsOK();
                break;
            case WITHDRAW_NSF:
                onWithdrawNSF();
                break;
            case UPDATE_DATABASE:
                onUpdateDatabase();
                break;
            case QUIT:
                onQuit();
                break;
            default:
                std::cout << "Unhandled message type." << std::endl;
                break;
        }
    }


public:
    MessageHandler(key_t sendKey, key_t receiveKey) {

        senderId = msgget(sendKey, 0666 | IPC_CREAT);
        if (senderId == -1) {
            std::perror("Error creating server queue");
        }
        clearOldMessage(senderId);

        // Create the client queue
        receiveId = msgget(receiveKey, 0666 | IPC_CREAT);
        if (receiveId == -1) {
            std::perror("Error creating server queue");
        }
        clearOldMessage(receiveId);
    }

    ~MessageHandler() {
        // Delete the server queue
        if (msgctl(senderId, IPC_RMID, nullptr) == -1) {
            std::perror("Error deleting server queue.");
        }

        // Delete the client queue
        if (msgctl(receiveId, IPC_RMID, nullptr) == -1) {
            std::perror("Error deleting client queue.");
        }
    } 

    // Methods to be overridden by derived classes
    virtual void onLoginRequest(std::string message) {
        std::cout << "onLoginRequest: " << message << std::endl;
    }

    virtual void onLoginPinWrong(std::string message) {
        std::cout << "onLoginPinWrong: " << message << std::endl;
    }

    virtual void onLoginLocked(std::string message) {
        std::cout << "onLoginLocked: " << message << std::endl;
    }

    virtual void onLoginSuccess(std::string message) {
        std::cout << "onLoginSuccess: " << message << std::endl;
    }

    virtual void onBalanceRequest() {
        std::cout << "Handling BALANCE_REQUEST in base class." << std::endl;
    }

    virtual void onBalanceFunds() {
        std::cout << "Handling BALANCE_FUNDS in base class." << std::endl;
    }

    virtual void onWithdrawRequest() {
        std::cout << "Handling WITHDRAW_REQUEST in base class." << std::endl;
    }

    virtual void onWithdrawFundsOK() {
        std::cout << "Handling WITHDRAW_FUNDS_OK in base class." << std::endl;
    }

    virtual void onWithdrawNSF() {
        std::cout << "Handling WITHDRAW_NSF in base class." << std::endl;
    }

    virtual void onUpdateDatabase() {
        std::cout << "Handling UPDATE_DATABASE in base class." << std::endl;
    }

    virtual void onQuit() {
        std::cout << "Handling QUIT in base class." << std::endl;
    }
};


#endif

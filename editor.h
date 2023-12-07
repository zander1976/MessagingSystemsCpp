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


#ifndef __EDITOR_H__
#define __EDITOR_H__

class EditorMessageHandler : public MessageHandler {
private: 
    sem_t *sem;

public:
    EditorMessageHandler(key_t sendKey, key_t receiveKey, sem_t *sem) : MessageHandler(sendKey, receiveKey) {
        this->sem = sem;
    }

    void UpdateDatabase() {
        std::cout << "\033[32mUpdating Database!\033[0m" << std::endl;
        sendMessage(UPDATE_DATABASE_REQUEST, "");
        receiveMessage();
    }

    virtual void onUpdateDatabaseComplete() {
        std::cout << "\033[32mDatabase Update Complete!\033[0m" << std::endl << std::endl;
    }
};


#endif // __EDITOR_H__
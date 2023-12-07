#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <string>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#ifndef __EDITOR_H__
#define __EDITOR_H__

class EditorMessageHandler : public MessageHandler {

public:
    EditorMessageHandler(key_t sendKey, key_t receiveKey) : MessageHandler(sendKey, receiveKey) {}

    void UpdateDatabase() {
        std::cout << "UpdateDatabase: "  << std::endl;
        sendMessage(UPDATE_DATABASE_REQUEST, "");
        receiveMessage();
    }

    virtual void onUpdateDatabaseComplete() {
        std::cout << "onUpdateDatabaseComplete: " << std::endl;
    }
};


#endif // __EDITOR_H__
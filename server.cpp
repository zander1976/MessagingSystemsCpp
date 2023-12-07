#include <iostream>
#include <sstream>
#include <cstring>
#include <map>
#include <iomanip>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <semaphore.h>

#include "messages.h"
#include "accounts.h"
#include "server.h"


int main() {

    // Create the semaphore
    sem_t *sem = sem_open("databasesem", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        std::perror("Semaphore creation/opening failed.");
        return 1;
    }
        
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

    key_t editorkey = ftok("./editor.cpp", 51);
    if (editorkey == -1) {
        std::perror("Editor Key error");
        exit(1);
    }

    ServerMessageHandler server(clientkey, serverKey, editorkey, sem);

    server.WaitForMessage();
    
    return 0;
}

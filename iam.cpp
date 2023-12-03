#include <iostream>
#include <unistd.h>

int main() {
    pid_t pid;
    int status;
    int value = 0;
    while(true) {
        std::cout << "Waiting for input: ";
        std::cin >> value;
        if (value == 1) {
            continue;
        }

        pid = fork();

        // Error
        if (pid < 0) {
            std::cerr << "Something went wrong" << std::endl;
            return 1;

        // The child
        } else if (pid == 0) {
            std::cout << "I am the child" << std::endl;
            execlp("/bin/ls", "la", NULL);
            
        // Parent
        } else {
            wait(&status);
            std::cout << "I am the parent" << std::endl;
        }
    }

    return 0;
}
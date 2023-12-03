#include <iostream>
#include <unistd.h>

int main() {
    pid_t pid;
    int status;
    int value = 0;

    pid = fork();

    // Error
    if (pid < 0) {
        std::cerr << "Something went wrong" << std::endl;
        return 1;

    // The child
    } else if (pid == 0) {
        std::cout << "Child waiting for input: ";
        std::cin >> value;
        if (value == 2) {
            std::cout << "Child Exit" << std::endl;
            return 0;
        }
        //execlp("/bin/ls", "la", NULL);
        
    // Parent
    } else {
        std::cout << "I am the parent" << std::endl;
        wait(&status);
        std::cout << "Parent Exit" << std::endl;
    }

    return 0;
}
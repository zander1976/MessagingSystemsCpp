#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

int main() {
    pid_t pid;
    int status;
    int *shared_value;
    const int SIZE = sizeof(int);

    // Create a semaphore 
    sem_t *sem = sem_open("benslock", O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        std::cerr << "Semaphore creation/opening failed.\n";
        return 1;
    }

    // Create and size shared memory
    int shm_fd = shm_open("bensmemory", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Error creating shared memory!" << std::endl;
        return 1;
    }
    ftruncate(shm_fd, SIZE);

    // Map the shared memory to the program
    shared_value = (int *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_value == MAP_FAILED) {
        std::cerr << "Map failed" << std::endl;
        return 1;
    }
    *shared_value = 0;

    // Fork a child process
    pid = fork();
    if (pid < 0) {
        std::cerr << "Fork failed" << std::endl;
        return 1;
    } else if (pid == 0) {
        while(true) {
            sem_wait(sem);
            std::cout << "Child Run" << std::endl;
        }
        std::cout << "Child Exit" << std::endl;
        return 0;
        
    } else {
        while(true) {
            std::cout << "I am the parent" << std::endl;
            std::cout << "Waiting for input: ";
            std::cin >> *shared_value;
            sem_post(sem);
        }

        wait(&status);
        std::cout << "Parent Exit" << std::endl;
    }

    return 0;
}

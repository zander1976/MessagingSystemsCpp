#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

int main() {
    int *shared_value;
    const int SIZE = sizeof(int);
    
    // Create the semaphore
    sem_t *sem = sem_open("benssemaphore", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        std::perror("Semaphore creation/opening failed.");
        return 1;
    }

    // Create and size shared memory
    int shm_fd = shm_open("bensint", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::perror("Error creating shared memory!");
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

    for(int i = 0; i < 6; i++) {
        sem_wait(sem);
        std::cout << "---------------------------"  << std::endl; 
        std::cout << "Critical Section: " << getpid() << std::endl; 
        (*shared_value)++;
        std::cout << "Shared Value: " << *shared_value << std::endl;
        sleep(2);
        std::cout << "Leave Section: " << getpid() << std::endl; 
        sem_post(sem);
        sleep(1);
    }
   
    int res = sem_close(sem);
    if (res != 0) {
        std::perror("Semaphore closing failed.");
        return 1;
    }    
    return 0;
}

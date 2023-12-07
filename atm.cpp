

int main() {
    // Create the semaphore
    sem_t *sem = sem_open("databasesem", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        std::perror("Semaphore creation/opening failed.");
        return 1;
    }

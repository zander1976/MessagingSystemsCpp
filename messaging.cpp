#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct msg_buffer {
    long msg_type;
    int msg_data;
};

int main() {
    pid_t pid1, pid2, pid3;
    int status;

    // Create the message queue
    key_t key = ftok("msgq", 65); // Generate a key
    int msgid = msgget(key, 0666 | IPC_CREAT); // Create a message queue

    pid1 = fork();

    // Error
    if (pid1 < 0) {
        std::cerr << "Something went wrong" << std::endl;
        return 1;

    // Parent (Process 1)
    } else if (pid1 == 0) {
        int num = 1;
        while (num <= 10) { // Limit the process to 10 iterations
            std::cout << "Process 1: " << num << std::endl;

            struct msg_buffer buf;
            buf.msg_type = 1;
            buf.msg_data = num;

            // Send number to Process 2
            msgsnd(msgid, &buf, sizeof(buf), 0);
            num++;
            sleep(1); // Introduce a delay for better observation
        }

    // Process 2 (First Child)
    } else {
        pid2 = fork();

        // Error
        if (pid2 < 0) {
            std::cerr << "Something went wrong" << std::endl;
            return 1;

        // First Child (Process 2)
        } else if (pid2 == 0) {
            struct msg_buffer buf;

            while (true) {
                // Receive number from Process 1
                msgrcv(msgid, &buf, sizeof(buf), 1, 0);
                int received_num = buf.msg_data;

                std::cout << "Process 2 (received): " << received_num << " Multiplied: " << received_num * 2 << std::endl;

                // Send multiplied number to Process 3
                buf.msg_data = received_num * 2;
                buf.msg_type = 2;
                msgsnd(msgid, &buf, sizeof(buf), 0);
            }

        // Process 3 (Second Child)
        } else {
            pid3 = fork();

            // Error
            if (pid3 < 0) {
                std::cerr << "Something went wrong" << std::endl;
                return 1;

            // Second Child (Process 3)
            } else if (pid3 == 0) {
                struct msg_buffer buf;

                while (true) {
                    // Receive multiplied number from Process 2
                    msgrcv(msgid, &buf, sizeof(buf), 2, 0);
                    int received_num = buf.msg_data;

                    std::cout << "Process 3 (received): " << received_num << " Incremented: " << received_num + 1 << std::endl;
                }
            }
        }
    }

    // Clean up the message queue
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}

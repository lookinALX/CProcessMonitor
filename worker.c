#include "common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

// Global flag for worker loop
volatile sig_atomic_t worker_running = 1;

// TODO: Implement socket creation and connection to server
// Создай TCP socket и подключись к серверу
// Верни socket file descriptor или -1 при ошибке
int connect_to_server(void) {
    // TODO: EXAM TOPIC - SOCKETS
    // 1. Создай socket с помощью socket(AF_INET, SOCK_STREAM, 0)
    // 2. Настрой struct sockaddr_in с адресом сервера
    // 3. Подключись с помощью connect()
    // 4. Верни socket fd при успехе
    
    printf("TODO: Implement socket creation and connection\n");
    return -1;
}

// TODO: Send monitoring data to server through socket
// Отправь структуру MonitorData на сервер
// Верни 0 при успехе, -1 при ошибке
int send_to_server(int sockfd, MonitorData *data) {
    // TODO: EXAM TOPIC - SOCKETS (sending data)
    // Используй send() или write() чтобы отправить данные через socket
    // Hint: отправь sizeof(MonitorData) байт
    
    printf("TODO: Implement sending data through socket\n");
    return -1;
}

// Collect CPU usage data from /proc/stat
void collect_cpu_data(MonitorData *data) {
    data->type = WORKER_CPU;
    data->timestamp = time(NULL);
    
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
        snprintf(data->data, BUFFER_SIZE, "ERROR: Cannot read /proc/stat");
        return;
    }
    
    // Read first line (cpu aggregated stats)
    if (fgets(data->data, BUFFER_SIZE, fp) == NULL) {
        snprintf(data->data, BUFFER_SIZE, "ERROR: Cannot parse /proc/stat");
    }
    fclose(fp);
}

// Collect memory info from /proc/meminfo
void collect_memory_data(MonitorData *data) {
    data->type = WORKER_MEMORY;
    data->timestamp = time(NULL);
    
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        snprintf(data->data, BUFFER_SIZE, "ERROR: Cannot read /proc/meminfo");
        return;
    }
    
    // Read first 3 lines (MemTotal, MemFree, MemAvailable)
    char line[256];
    data->data[0] = '\0';
    for (int i = 0; i < 3 && fgets(line, sizeof(line), fp); i++) {
        strncat(data->data, line, BUFFER_SIZE - strlen(data->data) - 1);
    }
    fclose(fp);
}

// Count running processes
void collect_process_data(MonitorData *data) {
    data->type = WORKER_PROCESSES;
    data->timestamp = time(NULL);
    
    FILE *fp = popen("ps aux | wc -l", "r");
    if (!fp) {
        snprintf(data->data, BUFFER_SIZE, "ERROR: Cannot execute ps command");
        return;
    }
    
    int count;
    if (fscanf(fp, "%d", &count) == 1) {
        snprintf(data->data, BUFFER_SIZE, "Running processes: %d", count - 1); // -1 for header
    } else {
        snprintf(data->data, BUFFER_SIZE, "ERROR: Cannot parse process count");
    }
    pclose(fp);
}

// Main worker function
void worker_main(WorkerType type, int cmd_pipe_fd, int ack_pipe_fd) {
    printf("[Worker %d] Started\n", type);
    
    // TODO: Connect to server
    int sockfd = connect_to_server();
    if (sockfd < 0) {
        fprintf(stderr, "[Worker %d] Failed to connect to server\n", type);
        exit(1);
    }
    
    printf("[Worker %d] Connected to server\n", type);
    
    MonitorData data;
    Command cmd;
    int active = 0;
    
    while (worker_running) {
        // TODO: EXAM TOPIC - PIPES (reading commands)
        // Прочитай команду из pipe (неблокирующее чтение или с timeout)
        // Hint: можешь использовать select() или poll() для timeout
        
        ssize_t n = read(cmd_pipe_fd, &cmd, sizeof(cmd));
        if (n > 0) {
            printf("[Worker %d] Received command: %d\n", type, cmd);
            
            switch (cmd) {
                case CMD_START:
                    active = 1;
                    // TODO: EXAM TOPIC - PIPES (sending ack)
                    // Отправь подтверждение parent процессу через ack_pipe_fd
                    write(ack_pipe_fd, "OK", 2);
                    break;
                    
                case CMD_STOP:
                    active = 0;
                    write(ack_pipe_fd, "OK", 2);
                    break;
                    
                case CMD_SHUTDOWN:
                    worker_running = 0;
                    break;
                    
                default:
                    break;
            }
        }
        
        // If active, collect and send data
        if (active) {
            // Collect data based on worker type
            switch (type) {
                case WORKER_CPU:
                    collect_cpu_data(&data);
                    break;
                case WORKER_MEMORY:
                    collect_memory_data(&data);
                    break;
                case WORKER_PROCESSES:
                    collect_process_data(&data);
                    break;
            }
            
            // Send to server
            if (send_to_server(sockfd, &data) < 0) {
                fprintf(stderr, "[Worker %d] Failed to send data to server\n", type);
            }
        }
        
        sleep(2); // Collection interval
    }
    
    // TODO: EXAM TOPIC - SOCKETS (cleanup)
    // Закрой socket с помощью close()
    printf("[Worker %d] Shutting down\n", type);
    close(sockfd);
    exit(0);
}

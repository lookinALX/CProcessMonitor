#include "common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

volatile sig_atomic_t server_running = 1;

void sigint_handler(int sig) {
    (void)sig;
    server_running = 0;
}

// TODO: Implement server socket creation and binding
// Создай server socket, привяжи его к адресу и начни слушать
// Верни socket fd или -1 при ошибке
int create_server_socket(void) {
    // TODO: EXAM TOPIC - SOCKETS (server side)
    // 1. Создай socket: socket(AF_INET, SOCK_STREAM, 0)
    // 2. Установи SO_REUSEADDR опцию с помощью setsockopt()
    // 3. Настрой struct sockaddr_in (INADDR_ANY, SERVER_PORT)
    // 4. Привяжи socket: bind()
    // 5. Начни слушать: listen() с backlog = 5
    // 6. Верни socket fd
    
    printf("TODO: Implement server socket creation\n");
    return -1;
}

// TODO: Accept client connection
// Прими входящее подключение от worker
// Верни client socket fd или -1 при ошибке
int accept_client(int server_fd) {
    // TODO: EXAM TOPIC - SOCKETS (accepting connections)
    // Используй accept() чтобы принять подключение
    // Можешь также вывести адрес подключившегося клиента
    
    printf("TODO: Implement accepting client connection\n");
    return -1;
}

// TODO: Receive data from client socket
// Прими данные MonitorData от worker через socket
// Верни 0 при успехе, -1 при ошибке или закрытии соединения
int receive_data(int client_fd, MonitorData *data) {
    // TODO: EXAM TOPIC - SOCKETS (receiving data)
    // Используй recv() или read() чтобы прочитать данные
    // Hint: читай sizeof(MonitorData) байт
    // Верни 0 при успехе, -1 если соединение закрыто
    
    printf("TODO: Implement receiving data from socket\n");
    return -1;
}

void print_data(MonitorData *data) {
    const char *type_str;
    switch (data->type) {
        case WORKER_CPU:       type_str = "CPU"; break;
        case WORKER_MEMORY:    type_str = "MEMORY"; break;
        case WORKER_PROCESSES: type_str = "PROCESSES"; break;
        default:               type_str = "UNKNOWN"; break;
    }
    
    printf("\n[%s @ %ld]\n%s\n", type_str, data->timestamp, data->data);
}

int main(void) {
    signal(SIGINT, sigint_handler);
    
    printf("=== Process Monitor Server ===\n");
    printf("Starting server on %s:%d\n\n", SERVER_HOST, SERVER_PORT);
    
    // Create and setup server socket
    int server_fd = create_server_socket();
    if (server_fd < 0) {
        fprintf(stderr, "Failed to create server socket\n");
        exit(1);
    }
    
    printf("Server listening... Waiting for workers to connect.\n\n");
    
    // Accept connections from 3 workers
    int client_fds[3] = {-1, -1, -1};
    int connected = 0;
    
    // TODO: Accept 3 worker connections
    while (connected < 3 && server_running) {
        int client_fd = accept_client(server_fd);
        if (client_fd >= 0) {
            client_fds[connected] = client_fd;
            printf("Worker %d connected (fd=%d)\n", connected, client_fd);
            connected++;
        }
    }
    
    if (connected < 3) {
        fprintf(stderr, "Not all workers connected\n");
        close(server_fd);
        exit(1);
    }
    
    printf("\nAll workers connected. Receiving data...\n");
    
    // Main server loop - receive data from workers
    MonitorData data;
    while (server_running) {
        // TODO: EXAM TOPIC - SOCKETS (multiplexing)
        // Опционально: используй select() или poll() чтобы 
        // мониторить все 3 socket'а одновременно
        // Пока что можно просто читать по очереди
        
        for (int i = 0; i < 3; i++) {
            if (client_fds[i] < 0) continue;
            
            int result = receive_data(client_fds[i], &data);
            if (result == 0) {
                print_data(&data);
            } else if (result < 0) {
                printf("Worker %d disconnected\n", i);
                close(client_fds[i]);
                client_fds[i] = -1;
            }
        }
        
        // Check if all workers disconnected
        int any_connected = 0;
        for (int i = 0; i < 3; i++) {
            if (client_fds[i] >= 0) {
                any_connected = 1;
                break;
            }
        }
        if (!any_connected) {
            printf("All workers disconnected\n");
            break;
        }
    }
    
    // Cleanup
    for (int i = 0; i < 3; i++) {
        if (client_fds[i] >= 0) {
            close(client_fds[i]);
        }
    }
    close(server_fd);
    
    printf("\nServer shutdown complete.\n");
    return 0;
}

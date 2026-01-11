#include "common.h"

// Global flag for graceful shutdown
volatile sig_atomic_t running = 1;

void sigint_handler(int sig) {
    (void)sig;
    running = 0;
}

// TODO: Implement pipe creation
// Создай два pipe: один для отправки команд (parent->child), 
// другой для получения подтверждений (child->parent)
// Верни 0 при успехе, -1 при ошибке
int create_pipes(PipePair *to_child, PipePair *to_parent) {
    // TODO: EXAM TOPIC - PIPES
    // Hint: используй pipe() системный вызов дважды
    // to_child: parent пишет в write_fd, child читает из read_fd
    // to_parent: child пишет в write_fd, parent читает из read_fd
    
    printf("TODO: Implement pipe creation\n");
    return -1;
}

// TODO: Implement worker process creation
// Создай child процесс с помощью fork()
// Верни PID child процесса в parent, 0 в child
pid_t create_worker(WorkerType type, PipePair *to_child, PipePair *to_parent) {
    // TODO: EXAM TOPIC - FORK
    // 1. Вызови fork()
    // 2. В child процессе:
    //    - Закрой ненужные концы pipes
    //    - Вызови worker_main() из worker.c
    // 3. В parent процессе:
    //    - Закрой ненужные концы pipes
    //    - Верни PID ребенка
    
    printf("TODO: Implement fork and worker creation\n");
    return -1;
}

// Send command to worker through pipe
int send_command(int pipe_fd, Command cmd) {
    // TODO: EXAM TOPIC - PIPES (writing)
    // Отправь команду через pipe используя write()
    // Верни количество записанных байт или -1 при ошибке
    
    printf("TODO: Implement command sending through pipe\n");
    return -1;
}

// Receive acknowledgment from worker through pipe
int receive_ack(int pipe_fd) {
    // TODO: EXAM TOPIC - PIPES (reading)
    // Прочитай подтверждение из pipe используя read()
    // Верни 0 при успехе, -1 при ошибке
    
    printf("TODO: Implement acknowledgment receiving through pipe\n");
    return -1;
}

int main(void) {
    signal(SIGINT, sigint_handler);
    
    printf("=== Process Monitor Coordinator ===\n");
    printf("Starting worker processes...\n\n");
    
    // Structures for communication
    PipePair to_workers[3];    // Pipes to send commands to workers
    PipePair from_workers[3];  // Pipes to receive acks from workers
    pid_t worker_pids[3];
    
    WorkerType types[] = {WORKER_CPU, WORKER_MEMORY, WORKER_PROCESSES};
    const char *type_names[] = {"CPU", "Memory", "Processes"};
    
    // TODO: Create pipes and fork workers
    for (int i = 0; i < 3; i++) {
        printf("Creating worker %d (%s)...\n", i, type_names[i]);
        
        // Create pipes for this worker
        if (create_pipes(&to_workers[i], &from_workers[i]) < 0) {
            fprintf(stderr, "Failed to create pipes for worker %d\n", i);
            exit(1);
        }
        
        // Fork worker process
        worker_pids[i] = create_worker(types[i], &to_workers[i], &from_workers[i]);
        if (worker_pids[i] < 0) {
            fprintf(stderr, "Failed to fork worker %d\n", i);
            exit(1);
        }
    }
    
    printf("\nAll workers started. Sending START command...\n");
    
    // Send START command to all workers
    for (int i = 0; i < 3; i++) {
        if (send_command(to_workers[i].write_fd, CMD_START) < 0) {
            fprintf(stderr, "Failed to send START to worker %d\n", i);
        }
    }
    
    // Wait for acknowledgments
    for (int i = 0; i < 3; i++) {
        if (receive_ack(from_workers[i].read_fd) == 0) {
            printf("Worker %d acknowledged START\n", i);
        }
    }
    
    printf("\nMonitoring... Press Ctrl+C to stop.\n\n");
    
    // Main loop
    while (running) {
        sleep(1);
    }
    
    printf("\nShutting down workers...\n");
    
    // TODO: Send SHUTDOWN command and wait for children
    for (int i = 0; i < 3; i++) {
        send_command(to_workers[i].write_fd, CMD_SHUTDOWN);
        
        // TODO: EXAM TOPIC - FORK (cleanup)
        // Используй waitpid() чтобы дождаться завершения child процесса
        // и избежать zombie процессов
        printf("TODO: Wait for worker %d to terminate\n", i);
    }
    
    printf("Coordinator shutdown complete.\n");
    return 0;
}

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

// Server configuration
#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

// Worker types
typedef enum {
    WORKER_CPU,
    WORKER_MEMORY,
    WORKER_PROCESSES
} WorkerType;

// Commands sent through pipes
typedef enum {
    CMD_START,
    CMD_STOP,
    CMD_CHANGE_INTERVAL,
    CMD_SHUTDOWN
} Command;

// Data structure sent to server
typedef struct {
    WorkerType type;
    long timestamp;
    char data[BUFFER_SIZE];
} MonitorData;

// Pipe structure for parent-child communication
typedef struct {
    int read_fd;
    int write_fd;
} PipePair;

#endif

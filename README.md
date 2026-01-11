# Process Monitor - C Programming Exam Project

Distributed process monitoring system for C programming exam preparation.

## Architecture

```
┌─────────────┐
│  Coordinator│  (parent process)
└──────┬──────┘
       │ fork() + pipes
       ├─────────────┬─────────────┬──────────────┐
       │             │             │              │
   ┌───▼────┐   ┌───▼────┐   ┌───▼────┐           │
   │Worker 1│   │Worker 2│   │Worker 3│           │
   │  (CPU) │   │ (MEM)  │   │ (PROC) │           │
   └───┬────┘   └───┬────┘   └───┬────┘           │
       │            │            │                │
       │ sockets    │ sockets    │ sockets        │
       └────────────┴────────────┴────────────── ─┘
                     │
                ┌────▼────┐
                │ Server  │  (receives data via TCP)
                └─────────┘
```

## Components

1. **Coordinator** - creates workers via fork(), manages them through pipes
2. **Workers** (3 processes) - collect system data, send to server via sockets
3. **Server** - receives data from workers via TCP sockets

## Exam Topics

### 1. PIPES (inter-process communication)
- `coordinator.c`: `create_pipes()` - creating pipes for parent-child communication
- `coordinator.c`: `send_command()` - sending commands through pipe
- `coordinator.c`: `receive_ack()` - receiving acknowledgments through pipe
- `worker.c`: reading commands and sending acknowledgments

### 2. FORK (process creation)
- `coordinator.c`: `create_worker()` - fork() to create child processes
- `coordinator.c`: `main()` - waitpid() for zombie process cleanup
- `worker.c`: `worker_main()` - code executed in child process

### 3. SOCKETS (network communication)
- **Client side** (`worker.c`):
  - `connect_to_server()` - creating socket and connecting to server
  - `send_to_server()` - sending data through socket
- **Server side** (`server.c`):
  - `create_server_socket()` - create, bind, listen
  - `accept_client()` - accepting connections
  - `receive_data()` - receiving data from clients

## How to Use

### 1. Build the project
```bash
make
```

### 2. Run
First, start the server in one terminal:
```bash
./server
```

Then start the coordinator in another terminal:
```bash
./coordinator
```

### 3. Stop
Press Ctrl+C in the coordinator window for graceful shutdown.

## What You Need to Implement

Find all `TODO: EXAM TOPIC` comments in the code and implement the functions:

### coordinator.c
- [ ] `create_pipes()` - creating pipes for communication
- [ ] `create_worker()` - fork() and child process setup
- [ ] `send_command()` - sending commands through pipe
- [ ] `receive_ack()` - receiving acknowledgments through pipe
- [ ] `main()` - waitpid() for cleanup

### worker.c
- [ ] `connect_to_server()` - creating TCP socket and connecting
- [ ] `send_to_server()` - sending data through socket
- [ ] Reading commands from pipe and sending acknowledgments

### server.c
- [ ] `create_server_socket()` - creating server socket (socket, bind, listen)
- [ ] `accept_client()` - accepting connections from workers
- [ ] `receive_data()` - receiving data through socket

## Useful System Calls

### Pipes
- `pipe(int pipefd[2])` - create pipe
- `read(fd, buf, size)` - read from pipe
- `write(fd, buf, size)` - write to pipe
- `close(fd)` - close file descriptor

### Fork
- `fork()` - create child process
- `waitpid(pid, &status, options)` - wait for child process termination
- `exit(status)` - terminate process

### Sockets (Client)
- `socket(AF_INET, SOCK_STREAM, 0)` - create socket
- `connect(sockfd, addr, addrlen)` - connect to server
- `send(sockfd, buf, len, flags)` or `write(sockfd, buf, len)`

### Sockets (Server)
- `socket(AF_INET, SOCK_STREAM, 0)` - create socket
- `setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, ...)` - set options
- `bind(sockfd, addr, addrlen)` - bind to address
- `listen(sockfd, backlog)` - start listening
- `accept(sockfd, addr, addrlen)` - accept connection
- `recv(sockfd, buf, len, flags)` or `read(sockfd, buf, len)`

## Data Structures

### struct sockaddr_in
```c
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(SERVER_PORT);
server_addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
```

## Testing

After implementing all TODO functions:
1. Start the server
2. Start the coordinator
3. Verify that all 3 workers connected to the server
4. Observe system data output every 2 seconds
5. Press Ctrl+C and verify graceful shutdown

## Additional Tasks (Optional)

- [ ] Add error handling for all system calls
- [ ] Implement CMD_CHANGE_INTERVAL to change data collection interval
- [ ] Use select()/poll() in server.c to monitor multiple sockets
- [ ] Add logging to files
- [ ] Handle SIGCHLD signal in coordinator to react to worker termination

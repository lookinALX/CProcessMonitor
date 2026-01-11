#ifndef WORKER_H
#define WORKER_H

#include "common.h"

// Main worker function called after fork
void worker_main(WorkerType type, int cmd_pipe_fd, int ack_pipe_fd);

#endif

#ifndef PTHREADFUNCS_H
#define PTHREADFUNCS_H

#include <pthread.h>
#include <sys/types.h>

#define COUNT_THREADS 4
#define COUNT_ITERATIONS 3

struct ThreadArgs {
    int id;
    char tag[10];
    char message[64];
};

extern int g_fd;
extern pthread_mutex_t g_lock;

pid_t getThreadID(void);
int write_line(const char *msg);
void *func_thread(void *arg);
void about(void);

#endif

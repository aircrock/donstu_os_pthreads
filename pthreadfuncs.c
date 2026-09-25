#include "pthreadfuncs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>

int g_fd = -1;
pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

pid_t getThreadID(void) {
    return (pid_t)syscall(SYS_gettid);
}

int write_line(const char *msg) {
    pthread_mutex_lock(&g_lock);

    size_t len = strlen(msg);
    size_t done = 0;
    int result = 0;

    while (done < len) {
        ssize_t n = write(g_fd, msg + done, len - done);
        if (n <= 0) {
            result = -1;
            break;
        }
        done += (size_t)n;
    }

    pthread_mutex_unlock(&g_lock);
    return result;
}

void *func_thread(void *arg) {
    struct ThreadArgs *t = arg;
    char buf[128];
    struct timespec delay = {0, 100000000L};

    if (write_line(t->message) != 0)
        perror("write_line");

    for (int i = 0; i < COUNT_ITERATIONS; i++) {
        snprintf(buf, sizeof(buf),
                 "[tag=%s] pid=%d tid=%d pthread=%lu iter=%d\n",
                 t->tag, getpid(), getThreadID(), (unsigned long)pthread_self(), i);

        if (write_line(buf) != 0)
            perror("write_line");

        nanosleep(&delay, NULL);
    }

    char *result = malloc(64);
    if (result == NULL)
        return NULL;

    snprintf(result, 64, "Iterations: %d", COUNT_ITERATIONS);
    return result;
}

void about(void) {
    printf("Pthread example\n");
}

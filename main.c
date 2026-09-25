#include "pthreadfuncs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    about();

    pthread_t threads[COUNT_THREADS];
    struct ThreadArgs args[COUNT_THREADS];
    char buf[128];

    g_fd = open("output.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (g_fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    snprintf(buf, sizeof(buf),
             "main: pid=%d, file=output.log, fd=%d\n",
             getpid(), g_fd);

    if (write_line(buf) != 0)
        perror("write_line");

    for (int i = 0; i < COUNT_THREADS; i++) {
        args[i].id = i + 1;
        snprintf(args[i].message,
                 sizeof(args[i].message),
                 "Hello from thread %d\n", i);
        snprintf(args[i].tag, sizeof(args[i].tag), "T%d", i);

        int rc = pthread_create(&threads[i], NULL,
                                func_thread, &args[i]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(rc));
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < COUNT_THREADS; i++) {
        void *ret = NULL;
        int rc = pthread_join(threads[i], &ret);
        if (rc == 0 && ret != NULL) {
            printf("Thread %d: %s\n", i, (char *)ret);
            free(ret);
        }
    }

    if (write_line("main: all threads finished\n") != 0)
        perror("write_line");

    close(g_fd);
    pthread_mutex_destroy(&g_lock);

    printf("main: all threads finished, file closed\n");
    return EXIT_SUCCESS;
}

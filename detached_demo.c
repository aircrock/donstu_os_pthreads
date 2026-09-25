#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct ThreadArgs {
    int id;
    char message[64];
};

void *worker(void *arg) {
    struct ThreadArgs *t = arg;

    printf("Detached thread %d: %s\n",
           t->id, t->message);

    free(t);
    return NULL;
}

int main(void) {
    pthread_t thread;

    struct ThreadArgs *args = malloc(sizeof(*args));
    if (args == NULL)
        return 1;

    args->id = 1;
    snprintf(args->message, sizeof(args->message),
             "Hello from malloc");

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(
        &attr, PTHREAD_CREATE_DETACHED
    );

    int rc = pthread_create(&thread, &attr, worker, args);
    pthread_attr_destroy(&attr);

    if (rc != 0) {
        free(args);
        return 1;
    }

    sleep(1);
    printf("Main finished\n");
    return 0;
}

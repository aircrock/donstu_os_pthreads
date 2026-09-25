#include <pthread.h>
#include <stdio.h>

void *worker(void *arg) {
    (void)arg;
    return NULL;
}

int main(void) {
    pthread_t t;
    pthread_create(&t, NULL, worker, NULL);

    printf("Same thread: %d\n",
           pthread_equal(pthread_self(), pthread_self()));

    printf("Different threads: %d\n",
           pthread_equal(pthread_self(), t));

    pthread_join(t, NULL);
    return 0;
}

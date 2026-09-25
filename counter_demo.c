#include <pthread.h>
#include <stdio.h>
#include <sched.h>

#define N 4
#define ITER 100000

int counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int use_mutex = 0;

void *worker(void *arg) {
    (void)arg;

    for (int i = 0; i < ITER; i++) {
        if (use_mutex)
            pthread_mutex_lock(&lock);

        counter++;

        if (use_mutex)
            pthread_mutex_unlock(&lock);

        if (i % 100 == 0)
            sched_yield();
    }

    return NULL;
}

void run_test(int mutex_enabled) {
    pthread_t threads[N];

    counter = 0;
    use_mutex = mutex_enabled;

    for (int i = 0; i < N; i++)
        pthread_create(&threads[i], NULL, worker, NULL);

    for (int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);

    printf("%s: counter = %d\n",
           mutex_enabled ? "С мьютексом" : "Без мьютекса",
           counter);
}

int main(void) {
    run_test(0);
    run_test(1);

    pthread_mutex_destroy(&lock);
    return 0;
}

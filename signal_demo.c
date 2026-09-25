#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

volatile sig_atomic_t quit_received = 0;

void quit_handler(int sig) {
    (void)sig;
    const char msg[] = "SIGQUIT received\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    quit_received = 1;
}

void *int_worker(void *arg) {
    (void)arg;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    int sig;
    sigwait(&set, &sig);

    printf("SIGINT received by sigwait: %d\n", sig);
    return NULL;
}

void *quit_worker(void *arg) {
    (void)arg;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);

    pthread_sigmask(SIG_UNBLOCK, &set, NULL);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    while (!quit_received)
        sigsuspend(&mask);

    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);

    pthread_sigmask(SIG_BLOCK, &set, NULL);

    struct sigaction sa = {0};
    sa.sa_handler = quit_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGQUIT, &sa, NULL);

    pthread_create(&t1, NULL, int_worker, NULL);
    pthread_create(&t2, NULL, quit_worker, NULL);

    printf("PID: %d\n", getpid());
    fflush(stdout);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}

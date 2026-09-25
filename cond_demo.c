#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef CAP
#define CAP 1
#endif

#define TOTAL 8

typedef struct {
    off_t offset;
    size_t len;
} Item;

int g_fd;
Item buffer[CAP];

int head = 0, tail = 0, count = 0;
off_t next_offset = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

#if CAP == 1
#define not_full not_empty
#else
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
#endif

void *producer(void *arg) {
    (void)arg;

    for (int i = 0; i < TOTAL; i++) {
        char msg[64];
        int len = snprintf(msg, sizeof(msg),
                           "Message %d\n", i);

        pthread_mutex_lock(&mutex);

        while (count == CAP)
            pthread_cond_wait(&not_full, &mutex);

        off_t offset = next_offset;

        if (pwrite(g_fd, msg, len, offset) != len) {
            perror("pwrite");
            exit(1);
        }

        next_offset += len;

        buffer[tail].offset = offset;
        buffer[tail].len = len;

        tail = (tail + 1) % CAP;
        count++;

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void *consumer(void *arg) {
    (void)arg;

    for (int i = 0; i < TOTAL; i++) {
        char msg[64];

        pthread_mutex_lock(&mutex);

        while (count == 0)
            pthread_cond_wait(&not_empty, &mutex);

        Item item = buffer[head];

        ssize_t n = pread(g_fd, msg,
                          item.len, item.offset);

        if (n != (ssize_t)item.len) {
            perror("pread");
            exit(1);
        }

        head = (head + 1) % CAP;
        count--;

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);

        msg[n] = '\0';
        printf("Consumer: %s", msg);
    }

    return NULL;
}

int main(void) {
    pthread_t p, c;

    g_fd = open("producer_consumer.log",
                O_RDWR | O_CREAT | O_TRUNC, 0644);

    if (g_fd < 0) {
        perror("open");
        return 1;
    }

    pthread_create(&p, NULL, producer, NULL);
    pthread_create(&c, NULL, consumer, NULL);

    pthread_join(p, NULL);
    pthread_join(c, NULL);

    close(g_fd);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_empty);

#if CAP > 1
    pthread_cond_destroy(&not_full);
#endif

    return 0;
}

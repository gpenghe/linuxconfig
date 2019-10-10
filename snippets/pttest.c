#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

void* thread_func(void* arg)
{
    printf("In thread_func\n");
    return NULL;
}

int main(void)
{
    pthread_t test_thread;

    int err = pthread_create(&test_thread, NULL, thread_func, NULL);
    if (err) {
        printf("Error creating thread: %s", strerror(err));
        return 1;
    }

    pthread_join(test_thread, NULL);

    printf("Done\n");

    return 0;
}

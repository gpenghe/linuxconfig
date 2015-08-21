#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* thread_func(void* thread_param)
{
    printf("in thread: thread_param:%d\n", *(int*)thread_param);
    *(int*)thread_param = 9;
    sleep(1);
    printf("thread done\n");
    return NULL;
}

int main(int argc, char** argv)
{
    pthread_t mythread;
    int x = 0;
    int y = 0;

    printf("x:%d, y:%d\n", x, y);
    if(pthread_create(&mythread, NULL, thread_func, &x)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    if(pthread_join(mythread, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        return 2;
    }
    printf("thead has been joined.\n");
    printf("x:%d, y:%d\n", x, y);

    return 0;
}

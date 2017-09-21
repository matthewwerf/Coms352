#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>

int value = 0;

extern void *hello(void);
extern void *world(void);

void main(int argc, char *argv[]){
	pthread_t tid;
	pthread_t tid2;

	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, world, NULL);
	pthread_create(&tid2, &attr, hello, NULL);
	pthread_join(tid, NULL);

	printf("\n");
}

void *world(void) {
	printf("world");
}

void *hello(void) {
	printf("Hello ");
}
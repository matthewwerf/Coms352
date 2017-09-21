#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>

extern void *hello(void *);
extern void *world(void *);

pthread_t tid2;

void main(int argc, char *argv[]){
	pthread_t tid;

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&tid, &attr, world, NULL);
	pthread_create(&tid2, &attr, hello, NULL);

	pthread_join(tid, NULL);

	printf("\n");
}

void *world(void *param) {
	pthread_yield(); //yield to ensure tid2 is set, if tid2 is uninitialized pthread_join executes instantly
	pthread_join(tid2, NULL);
	printf("world");
	pthread_exit(0);
}

void *hello(void *param) {
	printf("Hello ");
	pthread_exit(0);
}

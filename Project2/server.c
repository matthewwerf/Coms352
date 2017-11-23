#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "StringEncoder.c"
#include <pthread.h>
#include <openssl/sha.h>

#define PORT 8010
#define TRUE 1

void *threadHandler(void *connectedSocket);

int main(int argc, char *argv[]){
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	char buffer[1024] = {0};
	char *hello = "Hello from server";
      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // Allow 20 Connections
    if (listen(server_fd, 20) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(TRUE){
    	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                       (socklen_t*)&addrlen))<0)
    	{
        	perror("accept");
        	exit(EXIT_FAILURE);
    	}

	pthread_t clientHandlerThread;

	if(pthread_create(&clientHandlerThread, NULL, threadHandler, (void*)&new_socket) != 0) {
		perror("Thread to handle client could not be created");
		exit(EXIT_FAILURE);
	}
	/*
    	valread = read( new_socket , buffer, 1024);
    	printf("%s\n",buffer );
    	send(new_socket , hello , strlen(hello) , 0 );
    	printf("Hello message sent\n");
    	return 0;
    	*/
    }
    return 0;
}

void *threadHandler(void *connectedSocket) {
	int socket = *(int*)connectedSocket;
	char clientPayload[4096];
	memset(clientPayload, 0x0, 4096);

	read(socket, clientPayload, 4096);
	//read(socket, clientDigitalSignature, 4096);

	printf("%s\n", clientPayload);

	printf("Handling %d socket\n", socket);

	char* splitPayload = strtok(clientPayload, "/n");
	unsigned char tmp[SHA_DIGEST_LENGTH];
	char messageDigest[SHA_DIGEST_LENGTH * 2];

	printf("Payload split\n%s\n", splitPayload);

	memset(tmp, 0x0, SHA_DIGEST_LENGTH);
	memset(messageDigest, 0x0, SHA_DIGEST_LENGTH * 2);

	SHA1((unsigned char *)splitPayload, strlen(splitPayload), tmp);
	//SHA1((unsigned char*)clientMessage, strlen(clientMessage), tmp);

	for(int i=0; i < SHA_DIGEST_LENGTH; i++) {
		sprintf((char*)&(messageDigest[i*2]), "%02x", tmp[i]);
	}

	printf("Message Digest: %s\n", messageDigest);

	char* digitalSignature;
	digitalSignature[0] = '\0';
	digitalSignature = stringToEncodedAscii(messageDigest);

	printf("SERVER CALCULATED DIGITAL SIGNATURE:\n%s\n", digitalSignature);

	//char* clientDigitalSignature;	
	//clientDigitalSignature = strtok(NULL, "\0");
	splitPayload = strtok(NULL, "\0");

	printf("%s\n", splitPayload);
	printf("strtok is not the problem\n");

	if(strncmp(splitPayload, digitalSignature, strlen(digitalSignature)) == 0){
		send(socket, "TRUE", 4, 0);
	} else {
		send(socket, "FALSE", 5, 0);
	}

	free(digitalSignature);
	//free(splitPayload);

	close(socket);
	pthread_exit(0);
}

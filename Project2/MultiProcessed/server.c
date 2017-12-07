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

//void *threadHandler(void *connectedSocket);
void processHandler(int* connectedSocket);

int main(int argc, char *argv[]){
	// Socket Implentation Following Online Tutorials
	// Socket Variable Declarations
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
      
    // Socket Descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Configuration of Socket Address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    // Binding Socket Address to Socket Descriptor
    if (bind(server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Have Socket Listen On That Address and Allow 20 Connections Max
    if (listen(server_fd, 20) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // While Forever, when a new connection is initiated accept it
    while(TRUE){
    	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                       (socklen_t*)&addrlen))<0)
    	{
        	perror("accept");
        	exit(EXIT_FAILURE);
    	}

    	// Multithreaded approach
    	/*
	    // Initialize Handler Thread
		pthread_t clientHandlerThread;

		// Spawn thread and have it use the threadHandler function
		if(pthread_create(&clientHandlerThread, NULL, threadHandler, (void*)&new_socket) != 0) {
			perror("Thread to handle client could not be created");
			exit(EXIT_FAILURE);
		}
		*/

		if(fork() == 0) {
			processHandler(&new_socket);
		}
    }
    return 0;
}

void processHandler(int* connectedSocket) {
	int socket = *connectedSocket;
	char clientPayload[4096];
	memset(clientPayload, 0x0, 4096);

	// Read data from socket
	read(socket, clientPayload, 4096);

	printf("Responding to Socket: %d\n", socket);

	// Grab Message from Payload
	char* splitPayload;
	splitPayload = strtok(clientPayload, "\n");
	unsigned char tmp[SHA_DIGEST_LENGTH];
	char messageDigest[SHA_DIGEST_LENGTH * 2];

	memset(tmp, 0x0, SHA_DIGEST_LENGTH);
	memset(messageDigest, 0x0, SHA_DIGEST_LENGTH * 2);

	// Hash Message based on code from SHA1 tutorial online into message digest
	SHA1((unsigned char *)splitPayload, strlen(splitPayload), tmp);

	for(int i=0; i < SHA_DIGEST_LENGTH; i++) {
		sprintf((char*)&(messageDigest[i*2]), "%02x", tmp[i]);
	}

	// Generate digital signature using StringEncoder
	char* digitalSignature = (char *)malloc(4096);
	memset(digitalSignature, 0x0, 4096);
	digitalSignature = stringToEncodedAscii(messageDigest);

	// Grab Digital Signature from the rest of the payload
	splitPayload = strtok(NULL, "\0");

	// Compare and respond accordingly
	if(strncmp(splitPayload, digitalSignature, strlen(digitalSignature)) == 0){
		send(socket, "TRUE", 4, 0);
	} else {
		send(socket, "FALSE", 5, 0);
	}

	// Free heap variables
	free(digitalSignature);

	// Close Socket
	close(socket);
}

/*
// Thread Handler Function
void *threadHandler(void *connectedSocket) {
	// Pass Socket Descriptor into Function
	int socket = *(int*)connectedSocket;
	char clientPayload[4096];
	memset(clientPayload, 0x0, 4096);

	// Read data from socket
	read(socket, clientPayload, 4096);

	printf("Responding to Socket: %d\n", socket);

	// Grab Message from Payload
	char* splitPayload;
	splitPayload = strtok(clientPayload, "\n");
	unsigned char tmp[SHA_DIGEST_LENGTH];
	char messageDigest[SHA_DIGEST_LENGTH * 2];

	memset(tmp, 0x0, SHA_DIGEST_LENGTH);
	memset(messageDigest, 0x0, SHA_DIGEST_LENGTH * 2);

	// Hash Message based on code from SHA1 tutorial online into message digest
	SHA1((unsigned char *)splitPayload, strlen(splitPayload), tmp);

	for(int i=0; i < SHA_DIGEST_LENGTH; i++) {
		sprintf((char*)&(messageDigest[i*2]), "%02x", tmp[i]);
	}

	// Generate digital signature using StringEncoder
	char* digitalSignature = (char *)malloc(4096);
	memset(digitalSignature, 0x0, 4096);
	digitalSignature = stringToEncodedAscii(messageDigest);

	// Grab Digital Signature from the rest of the payload
	splitPayload = strtok(NULL, "\0");

	// Compare and respond accordingly
	if(strncmp(splitPayload, digitalSignature, strlen(digitalSignature)) == 0){
		send(socket, "TRUE", 4, 0);
	} else {
		send(socket, "FALSE", 5, 0);
	}

	// Free heap variables
	free(digitalSignature);

	// Close Socket
	close(socket);

	// Exit Thread
	pthread_exit(0);
}
*/

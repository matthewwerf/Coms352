#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include "StringEncoder.c"

#define PORT 8010

int main(int arg, char *argv[]) {
	struct sockaddr_in address;
	int sock = 0, valread;
	struct sockaddr_in serv_addr;

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket could not be created \n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("Invalid address\n");
		return -1;
	}

	if(connect(sock, (struct sockaddr * )&serv_addr, sizeof(serv_addr)) < 0){
		printf("Connection could not be established\n");
		return -1;
	}

	//char message[1024];
	char* message = (char *)malloc(1024);
	ssize_t messageSize = 1024;
	char response[1024];

	printf("Please provide message less than 1023 characters\n");
	getline(&message, &messageSize, stdin);

	unsigned char tmp[SHA_DIGEST_LENGTH];
	char messageDigest[SHA_DIGEST_LENGTH * 2];

	memset(tmp, 0x0, SHA_DIGEST_LENGTH);
	memset(messageDigest, 0x0, SHA_DIGEST_LENGTH * 2);

	SHA1((unsigned char *)message, strlen(message), tmp);

	for (int i=0; i < SHA_DIGEST_LENGTH; i++) {
		sprintf((char*)&(messageDigest[i*2]), "%02x", tmp[i]);
	}

	printf("Message Digest: %s\n", messageDigest);

	char* digitalSignature = stringToEncodedAscii(messageDigest);
	char* fullPayload = (char *)malloc(strlen(message) + strlen(digitalSignature) + 1);
	memset(fullPayload, 0x0, strlen(message) + strlen(digitalSignature) + 1);
	fullPayload[0] = '\0';
	strcat(fullPayload, message);
	strcat(fullPayload, digitalSignature);
	printf("payload size: %d\n", strlen(fullPayload));
	fullPayload[strlen(fullPayload)] = '\0';
	send(sock, fullPayload, strlen(fullPayload), 0);
	printf("FULL PAYLOAD: %s\n", fullPayload);
	//send(sock, message, strlen(message), 0);
	//send(sock, digitalSignature, strlen(digitalSignature), 0);
	free(digitalSignature);
	free(fullPayload);

	printf("Message send\n");
	valread = read(sock, response, 4096);
	printf("%s\n", response);
	close(sock);
	return 0;
}

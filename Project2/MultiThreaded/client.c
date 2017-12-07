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
	// Socket Implentation Following Online Tutorials
	// Socket Variable Declarations
	struct sockaddr_in address;
	int sock = 0, valread;
	struct sockaddr_in serv_addr;

	// Socket Desriptor
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket could not be created \n");
		return -1;
	}

	// Configuration of Socket Address
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Attempt Connection to server through localhost
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("Invalid address\n");
		return -1;
	}
	if(connect(sock, (struct sockaddr * )&serv_addr, sizeof(serv_addr)) < 0){
		printf("Connection could not be established\n");
		return -1;
	}

	// Allocation of Strings
	char message[1024];
	char response[1024];

	// Read line from stdin
	printf("Please provide message less than 1023 characters\n");
	fgets(message, 1024, stdin);

	// Allocate Strings for hashing and zero them (using actual 0 not '0')
	unsigned char tmp[SHA_DIGEST_LENGTH];
	char messageDigest[SHA_DIGEST_LENGTH * 2];
	memset(tmp, 0x0, SHA_DIGEST_LENGTH);
	memset(messageDigest, 0x0, SHA_DIGEST_LENGTH * 2);

	// Generate message digest based on code found from tutorial online
	SHA1((unsigned char *)message, strlen(message) - 1, tmp);

	for (int i=0; i < SHA_DIGEST_LENGTH; i++) {
		sprintf((char*)&(messageDigest[i*2]), "%02x", tmp[i]);
	}

	// General Digital Signature using StringEncoder
	char* digitalSignature = stringToEncodedAscii(messageDigest);

	// Allocate string to hold the entire payload so that it can be sent all at once
	char* fullPayload = (char *)malloc(strlen(message) + strlen(digitalSignature) + 1);
	memset(fullPayload, 0x0, strlen(message) + strlen(digitalSignature) + 1);

	// Append message first then digital signature
	strcat(fullPayload, message);
	strcat(fullPayload, digitalSignature);
	
	// Add null terminating character just in case
	fullPayload[strlen(fullPayload)] = '\0';

	// Send Payload
	send(sock, fullPayload, strlen(fullPayload), 0);
	
	// Free heap variables
	free(digitalSignature);
	free(fullPayload);

	printf("Message send\n");
	valread = read(sock, response, 4096);

	// Print Server Response
	printf("%s\n", response);

	// Close Socket Connection
	close(sock);
	return 0;
}

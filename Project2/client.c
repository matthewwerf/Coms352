#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>


#define PORT 9999

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

	char message[1024];
	char response[1024];

	printf("Please provide message less than 1023 characters\n");
	fgets(message, sizeof(message), stdin);

	char messageDigest[SHA_DIGEST_LENGTH];
	SHA1(message, sizeof(message), messageDigest);

	char digitalSignature[SHA_DIGEST_LENGTH];
	

	printf("%s\n", hash);

	send(sock, message, strlen(message), 1024);
	printf("Message send\n");
	valread = read(sock, response, 1024);
	printf("%s\n", response);
	return 0;
}

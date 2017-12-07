all: client server

client: client.c
	gcc -o client client.c -lm -lcrypto
server: server.c
	gcc -o server server.c -lm -lpthread -lcrypto
clean:
	$(RM) client server		

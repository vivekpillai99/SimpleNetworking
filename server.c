#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <stdlib.h>

#define MAX_QUEUE 5
#define MAX_SIZE 1024

struct packet{
	int size;
	char buffer[MAX_SIZE];
};

int main(int argc, char *argv[]){
	struct sockaddr_in sin;
	struct packet packet_in;
	int len;
	int sock;
	int new_sock;
	int server_port = 0;
	
	if(argc < 2){
		printf("Please input a port number\n");
		exit(0);
	}
	//grab port number from commandline
	server_port = atoi(argv[1]);
	//zero out the struct
	bzero((char *)&sin, sizeof(sin));
	//set up the socket in
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(server_port);

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		printf("Unable to establish socket connection, exiting...\n");
		exit(-1);
	}

	if((bind(sock, (struct sockaddr *)&sin, sizeof(sin))) < 0){
		printf("bind failed, exiting...\n");
		exit(-1);
	}
	listen(sock, MAX_QUEUE);
	int size = 0;
	for( ; ; ){//accept connections until told otherwise
		new_sock = accept(sock, (struct sockaddr *)&sin, &len);
		if(new_sock < 0){
			printf("Failed to accept new socket, exiting...\n");
			exit(-1);
		}
		while(len = recv(new_sock, &packet_in, sizeof(int)+MAX_SIZE, 0)){
			//while the connection is alive recieve messages
			if(len != 1028){
		 		break;
			}
			printf("%d\n", packet_in.size);
			int i;
			for (i = 0; i < packet_in.size; i++){
			//since the packet is transmitted without the null terminator loop 
			//through the size of the message
				printf("%c", packet_in.buffer[i]);
			}
			printf("\n");
		}
		close(new_sock);
		exit(0);
	}
}

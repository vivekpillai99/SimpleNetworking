#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BYTES 1024
int port;
char inbuff[MAX_BYTES+1];

struct packet{
	int size;
	char buff[MAX_BYTES];
};


int main(int argc, char *argv[]){
	FILE *fp;
	struct in_addr addr;
	struct sockaddr_in sin;
	char *host;
	struct packet out;
	int sock;
	
	if (argc >= 3){
		port = atoi(argv[2]); 
	}
	else{
		fprintf(stderr, "ERROR: No Port number or IP\n");
		exit(1);
	}
 	if (inet_aton(argv[1], &addr) == 0){
		fprintf(stderr, "ERROR: Invalid address\n");
		exit(1);
	}
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = PF_INET;
	sin.sin_port = htons(port);
        bcopy(&addr, (char *)&sin.sin_addr, sizeof(addr));	

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0){	
		fprintf(stderr, "ERROR: Unable to establish socket connection\n");
		exit(1);
	}
	
	if (connect(sock, (struct sockaddr*)&sin, sizeof(sin)) < 0){
		fprintf(stderr, "ERROR: Unable to connect to socket\n");
		close(sock);
		exit(1);
	}
	int len =0;
        while (fgets(inbuff, MAX_BYTES + 1, stdin)){
		len =strlen(inbuff);
		//Need to determine if buffer is maxed out or just newline at the end to ignore.
		if (len == 1024){
			if (inbuff[len-1] == '\n'){
				out.size = len - 1;
			}
			else{
				out.size = len;
			}	
		}
		else{
			//This ignores the newline that fgets produces
			out.size = len-1;
		}
		//Transfer input to packet buffer
		memcpy(out.buff, inbuff, MAX_BYTES);
		send(sock, &out, sizeof(struct packet), 0);
		if (out.size >= 1024){
			while ((getchar()) != '\n'); 
		}
		//Zero out buffers for the next input
		bzero((char *)&out.buff, 1024);
		bzero((char *)&inbuff, 1025);
	}	
}

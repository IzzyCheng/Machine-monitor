#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{
	//create socket
	int sock = 0;
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
		printf("Fail to create a socket.");

	//connect socket
	struct sockaddr_in info;
	bzero(&info, sizeof(info));
	info.sin_family = PF_INET;
	info.sin_addr.s_addr = inet_addr("127.0.0.1");
	info.sin_port = htons(1104);

	int err = connect(sock, (struct sockaddr *)&info, sizeof(info));
	if (err == -1)
		printf("connection error");

	//Get request type
	char which[1]= {};
	char pid[10] = {};
	printf("========================================================\n");
	printf("(a)list all process ids\n");
	printf("(b)thread's IDs\n");
	printf("(c)child's PIDs\n");
	printf("(d)process name\n");
	printf("(e)state of process(D,R,S,T,t,W,X,Z)\n");
	printf("(f)command line of excuting process(cmdline)\n");
	printf("(g)parent's PID\n");
	printf("(h)all ancients of PIDs\n");
	printf("(i)virtual memory size(VmSize)\n");
	printf("(j)physical memory size(VmRSS)\n");
	printf("(k)exit\n");
	printf("which? ");
	scanf("%s", which);
	if (which[0] != 'a' && which[0] != 'k') {
		printf("pid? ");
		scanf("%s", pid);
	}

	//Send & Recv
	char receive[1000] = {};
	char message[20] = {};
	strcat(message, which);
	strcat(message, pid);
	send(sock, message, sizeof(message), 0);
	recv(sock, receive, sizeof(receive), 0);
	switch(which[0]) {
	case 'a':
		printf("[listAll]: %s\n", receive);
		break;
	case 'b':
		printf("[thread ID]: %s\n", receive);
		break;
	case 'c':
		printf("[child PID]: %s\n", receive);
		break;
	case 'd':
		printf("[process name]: %s\n", receive);
		break;
	case 'e':
		printf("[state of Process]: %s\n", receive);
		break;
	case 'f':
		printf("[cmdline]: %s\n", receive);
		break;
	case 'g':
		printf("[paraent PID]: %s\n", receive);
		break;
	case 'h':
		printf("[all Ancient]: %s\n", receive);
		break;
	case 'i':
		printf("[VmSize]: %s\n", receive);
		break;
	case 'j':
		printf("[VmRSS]: %s\n", receive);
		break;
	case 'k':
		close(sock);
		break;
	}
	close(sock);

	return 0;
}


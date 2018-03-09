#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <dirent.h>

void *service(void *Csock)
{
	//recv input from client
	char input[10] = {};
	int *ClientSock = Csock;
	recv(*ClientSock, input, sizeof(input), 0);
	printf("Get %s in thread\n", input);

	//get pid
	char pid[5] = {};
	for (int i = 1; i<strlen(input); i++)
		pid[i-1] = input[i];

	//serve depend on which
	char result[1000] = {};
	switch(input[0]) {
	case 'a': {     //list all process ids
		struct dirent* dir;         //used to read dir name in /proc
		DIR*dp = opendir("/proc");
		if (dp == NULL)
			printf("Fail to open dir\n");
		while ((dir = readdir(dp)) != NULL) {   //read dir name
			int pid = atoi(dir->d_name);
			if (pid != 0) {         //if id != 0 put into result
				strcat(result, dir->d_name);
				strcat(result, ", ");
			}
		}
		break;
	}//////////////////////////////////////////////////////////
	case 'b': {     //thread's ID
		//path of process status /proc/[pid]/status
		char path[30] = {};
		strcat(path, "/proc/");
		strcat(path, pid);
		strcat(path, "/status");
		FILE *infile;
		infile = fopen(path, "r");
		if (infile == NULL)
			printf("Wrong process ID!\n");

		//search Pid for thread's ID
		char search[100];
		while(fgets(search, 100, infile) != NULL) {
			char *loc = strstr(search, "Pid");
			if (loc != NULL) {
				//temp to store searched array start from loc
				char temp[15] = {};
				sprintf(temp, "%s", loc);

				//get thread's ID and put into result
				for (int i = 4, j = 0; i<strlen(temp); i++)
					if (temp[i] != '\t')
						if (temp[i] == '\n')
							break;  //break while
						else {
							result[j] = temp[i];
							j++;
						}
				fclose(infile);
				break;  //break while
			}
		}
		break;
	}//////////////////////////////////////////////////////////
	case 'c': {     //child's PIDs
		//path of children's PIDs : /proc/[pid]/task/[pid]/children
		char path[30] = {};
		strcat(path, "/proc/");
		strcat(path, pid);
		strcat(path, "/task/");
		strcat(path, pid);
		strcat(path, "/children");
		FILE *infile;
		infile = fopen(path, "r");
		if (infile == NULL)
			printf("Wrong process ID!\n");

		//get all Info
		fgets(result, 1000, infile);
		fclose(infile);
		break;
	}//////////////////////////////////////////////////////////
	case 'd': {     //process name
		//Almost same as (b)
		char path[30] = {};
		strcat(path, "/proc/");
		strcat(path, pid);
		strcat(path, "/status");
		FILE *infile;
		infile = fopen(path, "r");
		if (infile == NULL)
			printf("Wrong process ID!\n");
		char search[100];
		while(fgets(search, 100, infile) != NULL) {
			char *loc = strstr(search, "Name");
			if (loc != NULL) {
				char temp[30] = {};
				sprintf(temp, "%s", loc);
				for (int i = 5, j = 0; i<strlen(temp); i++)
					if (temp[i] != '\t')
						if (temp[i] == '\n')
							break;
						else {
							result[j] = temp[i];
							j++;
						}
				fclose(infile);
				break;
			}
		}
		break;
	}//////////////////////////////////////////////////////////
	case 'e': {     //state of process
		//Almost same as (b)
		char path[30] = {};
		strcat(path, "/proc/");
		strcat(path, pid);
		strcat(path, "/status");
		FILE *infile;
		infile = fopen(path, "r");
		if (infile == NULL)
			printf("Wrong process ID!\n");
		char search[100];
		while(fgets(search, 100, infile) != NULL) {
			char *loc = strstr(search, "State");
			if (loc != NULL) {
				char temp[30] = {};
				sprintf(temp, "%s", loc);
				result[0] = temp[7];        //just take one char
				fclose(infile);
				break;
			}
		}
		break;
	}//////////////////////////////////////////////////////////
	case 'f': {     //command line of excuting process
		//path of cmdine : /proc/[pid]/cmdline
		char path[30] = {};
		strcat(path, "/proc/");
		strcat(path, pid);
		strcat(path, "/cmdline");
		FILE *infile;
		infile = fopen(path, "r");
		if (infile == NULL)
			printf("Wrong process ID!\n");

		//get all Info
		fgets(result, 100, infile);
		fclose(infile);
		break;
	}//////////////////////////////////////////////////////////
	case 'g': {     //parent's PID
		//Almost same as (b)
		char path[30] = {};
		strcat(path, "/proc/");
		strcat(path, pid);
		strcat(path, "/status");
		FILE *infile;
		infile = fopen(path, "r");
		if (infile == NULL)
			printf("Wrong process ID!\n");
		char search[100];
		while(fgets(search, 100, infile) != NULL) {
			char *loc = strstr(search, "PPid");
			if (loc != NULL) {
				char temp[15] = {};
				sprintf(temp, "%s", loc);
				for (int i = 5, j = 0; i<strlen(temp); i++)
					if (temp[i] != '\t')
						if (temp[i] == '\n')
							break;
						else {
							result[j] = temp[i];
							j++;
						}
				fclose(infile);
				break;
			}
		}
		break;
	}//////////////////////////////////////////////////////////
	case 'h': {     //all ancients of PID
		char path[30] = {};
		//find ancient pid loop (same as (g))
		while (1) {
			char temp_result[10] = {};
			char path[30] = {};
			strcat(path, "/proc/");
			strcat(path, pid);
			strcat(path, "/status");
			FILE *infile;
			infile = fopen(path, "r");
			if (infile == NULL)
				printf("Wrong process ID!\n");
			char search[100];
			while(fgets(search, 100, infile) != NULL) {
				char *loc = strstr(search, "PPid");
				if (loc != NULL) {
					char temp[15] = {};
					sprintf(temp, "%s", loc);
					for (int i = 5, j = 0; i<strlen(temp); i++)
						if (temp[i] != '\t')
							if (temp[i] == '\n')
								break;
							else {
								temp_result[j] = temp[i];
								j++;
							}
					fclose(infile);
					break;
				}
			}
			strcpy(pid, "");            //clear old pid
			strcat(pid, temp_result);   //set pid to parent PID
			strcat(result, pid);        //put parent PID into result
			//there is no parents above
			if (pid[0] == '0')
				break;
			strcat(result, "->");
		}
		break;
	}//////////////////////////////////////////////////////////
	case 'i': {     //virtual memory size(VmSize)
		//Almost same as (b)
		char path[30] = {};
		strcat(path, "/proc/");
		strcat(path, pid);
		strcat(path, "/status");
		FILE *infile;
		infile = fopen(path, "r");
		if (infile == NULL)
			printf("Wrong process ID!\n");
		char search[100];
		while(fgets(search, 100, infile) != NULL) {
			char *loc = strstr(search, "VmSize");
			if (loc != NULL) {
				char temp[30] = {};
				sprintf(temp, "%s", loc);
				for (int i = 7, j = 0; i<strlen(temp); i++)
					if (temp[i] != '\t')
						if (temp[i] == '\n')
							break;
						else {
							result[j] = temp[i];
							j++;
						}
				fclose(infile);
				break;
			}
		}
		break;
	}//////////////////////////////////////////////////////////
	case 'j': {     //physical memory size(VmRSS)
		//Almost same as (b)
		char path[30] = {};
		strcat(path, "/proc/");
		strcat(path, pid);
		strcat(path, "/status");
		FILE *infile;
		infile = fopen(path, "r");
		if (infile == NULL)
			printf("Wrong process ID!\n");
		char search[100];
		while(fgets(search, 100, infile) != NULL) {
			char *loc = strstr(search, "VmRSS");
			if (loc != NULL) {
				char temp[30] = {};
				sprintf(temp, "%s", loc);
				for (int i = 6, j = 0; i<strlen(temp); i++)
					if (temp[i] != '\t')
						if (temp[i] == '\n')
							break;
						else {
							result[j] = temp[i];
							j++;
						}
				fclose(infile);
				break;
			}
		}
		break;
	}//////////////////////////////////////////////////////////
	}

	send(*ClientSock, result, sizeof(result), 0);
	pthread_exit(NULL);
	return NULL;
}

void createThread(int* Csock)
{
	pthread_t id;
	pthread_create(&id, NULL, &service, Csock);
}

int main(int argc, char **argv)
{
	//Create socket
	char inputBuf[256] = {};
	int sock = 0, forClientsock = 0;
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
		printf("Fail to create a socket\n");

	//Connetc socket
	struct sockaddr_in serverInfo, clientInfo;
	int addrlen = sizeof(clientInfo);
	bzero(&serverInfo, sizeof(serverInfo));

	serverInfo.sin_family = PF_INET;
	serverInfo.sin_addr.s_addr = INADDR_ANY;
	serverInfo.sin_port = htons(1104);
	bind(sock, (struct sockaddr *)&serverInfo, sizeof(serverInfo));
	listen(sock, 5);

	//accept request and create thread to serve it
	while(1) {
		forClientsock = accept(sock, (struct sockaddr*)&clientInfo, &addrlen);
		int *Clientsockptr = (int *)malloc(sizeof(int));
		*Clientsockptr = forClientsock;
		createThread(Clientsockptr);
	}

	return 0;
}













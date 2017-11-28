// Connor Nelson (1208559452)
// Denim Datta (1213152289)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "threads.h"
#include "msgs.h"

#define DHCP_SERVER 0
#define DHCP_CLIENT 1
#define FILE_SERVER 5
#define FILE_CLIENT_BEGIN 11

#define MAX_CONCURRENT_TRANSFER 3
#define READ_SIZE 255

#define FILE_FAILURE -1
#define FILE_CONTINUE 0
#define FILE_SUCCESS 1

char** filenames;
sem_t transferSem;

void dhcp_server(void);
void file_server(void);
void file_client(void);

int main(int argc, char *argv[])
{
	printf("\n");
	int client_no = (*argv[1]) - '0';
	filenames = calloc(client_no, sizeof(char*));
    for (int i = 2; i < argc; i++) {
		filenames[i-2] = argv[i];
    }
	init_sem(&transferSem, MAX_CONCURRENT_TRANSFER);
    start_thread(dhcp_server);
    start_thread(file_server);
    for (int i = 1; i <= client_no; ++i)
        start_thread(file_client);
    run();
    return 0;
}

void dhcp_server(void)
{
    int available_port_index = FILE_CLIENT_BEGIN;
    int number_of_ports = sizeof(ports) / sizeof(port_t);

    while (1) {
        message_t request = receive(&ports[DHCP_SERVER]);

        message_t response;
        memset(&response, 0, sizeof(response));
        response.integers[0] = DHCP_SERVER;
        if (available_port_index < number_of_ports) {
            response.integers[1] = available_port_index;
            available_port_index += 1;
        }
        else {
            response.integers[1] = -1;
        }
        send(&ports[DHCP_CLIENT], response);
    }
}

int request_port(void)
{
    message_t request;
    memset(&request, 0, sizeof(request));
    request.integers[0] = DHCP_CLIENT;

    send(&ports[DHCP_SERVER], request);
    message_t response = receive(&ports[DHCP_CLIENT]);
    int port = response.integers[1];
    return port;
}

void file_server(void)
{
	printf("\t\t\t\t\t\t\t[Server %d] Server Started \n", FILE_SERVER);
    while (1) {
        message_t request = receive(&ports[FILE_SERVER]);
        int reply_port = request.integers[0];
        int type = request.integers[1];
		printf("\t\t\t\t\t\t\t[Server %d] received data from [Client %d] \n", FILE_SERVER, reply_port);
		if(strlen(request.messagebody[0]) > 15) {
			printf("\t\t\t\t\t\t\t[Server %d] filename %s name length %d, exceeding limit 15 [Client %d]\n", FILE_SERVER, request.messagebody[0], strlen(request.messagebody[0]), reply_port);
			message_t response;
			memset(&response, 0, sizeof(response));
			response.integers[1] = FILE_FAILURE;
			response.messagebody[0] = calloc(strlen("Filename exceeds 15 characters")+1, sizeof(char));
			memset(response.messagebody[0],0,sizeof(response.messagebody[0]));
			response.messagebody[0] = "Filename exceeds 15 characters";
			send(&ports[reply_port], response);
			continue;
		}
		
		//Exceeding 1 MB (1000000)
		if(request.integers[2] > 1000000) {
			printf("\t\t\t\t\t\t\t[Server %d] filesize exceeding 1MB for [Client %d]\n", FILE_SERVER, reply_port, request.messagebody[0]);
			message_t response;
			memset(&response, 0, sizeof(response));
			response.integers[1] = FILE_FAILURE;
			response.messagebody[0] = calloc(strlen("File size exceeds 1 MB")+1, sizeof(char));
			memset(response.messagebody[0],0,sizeof(response.messagebody[0]));
			response.messagebody[0] = "File size exceeds 1 MB";
			send(&ports[reply_port], response);
			continue;
		}
		
		char* filename = calloc(strlen(request.messagebody[0])+strlen(".server")+1, sizeof(char));
		memset(filename, 0, sizeof(filename));
		strcat(filename, request.messagebody[0]);
		strcat(filename, ".server");
        
		// In first request, if file already present delete the file
		if (request.integers[3] == -1)
			remove(filename);
		
		FILE* fp = fopen(filename, "a");
		fputs(request.messagebody[1], fp);
		fclose(fp);
		
        message_t response;
        memset(&response, 0, sizeof(response));
        response.integers[0] = 0;
        response.integers[1] = FILE_CONTINUE;
        send(&ports[reply_port], response);
    }
}

void file_client(void)
{
    int client_port = request_port();
    if (client_port < 0) {
        puts("No more ports available, client terminated");
        // Never actually implemented thread termination...
        while (1)
            yield();
    }
    printf("Client got port [%d] from DHCP server\n", client_port);
	char* filename = filenames[client_port - FILE_CLIENT_BEGIN];
	
	p(&transferSem);
    
	FILE * fp = fopen(filename, "r");
	
	if(fp == NULL) {
		printf("[Client %d] Client file %s doesn't exist\n", client_port, filename);
		// Never actually implemented thread termination...
		while(1) {
			yield();
		}
	}
	
	fseek(fp, 0L, SEEK_END);
	int filesize = ftell(fp);
	rewind(fp);
	
	int firstMsg = -1;
	
	while (1) {
		printf("[Client %d] Sending data to server\n", client_port);

		message_t request;
		memset(&request, 0, sizeof(request));
		request.integers[0] = client_port;
		request.integers[1] = FILE_CONTINUE;
		request.integers[2] = filesize;
		request.integers[3] = firstMsg;
		if(firstMsg == -1) {
			firstMsg = 1;
		}
		request.messagebody[0] = filename;
		char buff[READ_SIZE];
		memset(buff, 0, sizeof(buff));
		if(fgets(buff, sizeof(buff), fp) == NULL) {
			printf("[Client %d] File transfer Complete.\n", client_port);
			break;
		}
		request.messagebody[1] = buff;
		send(&ports[FILE_SERVER], request);
		message_t response = receive(&ports[client_port]);
		
		if(response.integers[1] == FILE_FAILURE) {
			printf("[Client %d] File can not be transferred.\n\t\t%s\n", client_port, response.messagebody[0]);
			break;
		}
		usleep(1 * 1000000);
	}
	
	fclose(fp);
	
	v(&transferSem);
	
	// Never actually implemented thread termination...
	while(1) {
		yield();
	}
}
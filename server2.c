/*
 * server2.c
 *
 *  Created on: Jan 29, 2022
 *      Author: Kartikeya
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

//Thread function for the TCP server
void *tcp_server_thread_2(void){

			char *ip = "192.168.1.50";
			int port = 5655;

			int server_sock, client_sock;
			struct sockaddr_in server_addr, client_addr;
			socklen_t addr_size;
			char buffer[1024]; //variable to store infomation transfered between client and server
			int n;

			//Creating a TCP server socket

			server_sock = socket(AF_INET, SOCK_STREAM, 0);
			if(server_sock < 0){
				perror("[-]Socket error...\n");
				exit(1);
			}
			printf("[+]TCP server socket created...\n");

			memset(&server_addr, '\0', sizeof(server_addr));

			//assigning port and IP
			server_addr.sin_family = AF_INET;             // this means IPv4
			server_addr.sin_port = port;				  // port we have defined
			server_addr.sin_addr.s_addr = inet_addr(ip);  // local ip address

			//Binding the newly created socket to the given IPP address
			n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
			if(n<0){
				perror("[-]Socket bind failed...\n");
				exit(1);
			}
			printf("[+]Socket successfully binded to the port %d\n",port);

			//Now the server is ready to listen
			listen(server_sock,5);
			printf("Listening...\n");

			while(1){
				//Accepting connection from client
				addr_size =sizeof(client_addr);
				client_sock =accept(server_sock, (struct sockaddr*)&client_addr,&addr_size);
				printf("[+]Client connected...\n");

				//Clearing the buffer
				bzero(buffer, 1024);
				//Receive message from client
				recv(client_sock, buffer, sizeof(buffer),0);
				printf("Client: %s\n",buffer);

				//Clearing the buffer
				bzero(buffer, 1024);
				strcpy(buffer,"Hi this is the server\n");
				printf("Server: %s\n", buffer);
				//Sending message to the client
				send(client_sock,buffer,strlen(buffer),0);

				close(client_sock);
				printf("[+]Client disconnected...\n\n");
			}
			return 0;

	return NULL;
}




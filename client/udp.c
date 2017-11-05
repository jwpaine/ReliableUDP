
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h> // sleep()
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h> // strcpy
#include <semaphore.h>

#include "client.h"

int udp_port, num_packets, filesize;

pthread_t tcp, udp;
struct sockaddr_in server;
char server_ip[16];


void udp_start(int port, int n_packets, int f_size, char ip[]) {
	/* set parameters and than indicate that the UDP thread should proceed */
	udp_port = port;
	num_packets = n_packets;
	filesize = f_size;
	strcpy(server_ip, ip);
	char *pos;
	if ((pos=strchr(server_ip, '\n')) != NULL) {
   	 *pos = '\0';
   	}


	/* start UDP thread */
	long t;
	int rc = pthread_create(&udp, NULL, UDP, (void*)t);
	pthread_detach(udp);

	if (rc) {
		printf("Error creating udp thread\n");
		exit(0);	
	} 
}

void *UDP(void *p) {


	printf("Started UDP thread\n");
	struct sockaddr_in server, local;
	int expected = 0; // expected packet

	int	err;	
 	char msg[1050] = {0}; // buffer for new incoming messages

 	int my_socket = socket(AF_INET, SOCK_DGRAM, 0);

 	// local
 	memset(&local, 0, sizeof(struct sockaddr_in));
   
    local.sin_family = AF_INET;
    local.sin_port = htons(10000);
 //   local.sin_addr.s_addr = inet_addr("127.0.0.1");
    local.sin_addr.s_addr = htonl(INADDR_ANY); 

 //   int ret = bind(my_socket, (struct sockaddr *)&local, sizeof(struct sockaddr));

 /*   // remote
    printf("Server IP set to %s\n", server_ip);
  	server.sin_family = AF_INET;
  	server.sin_port = htons(udp_port);
  	err = inet_pton(AF_INET, server_ip, &(server.sin_addr)); 

  	 if (err <1 ) {
 		printf("inet_pton");
 		exit(0);
	}
	// setup socket 
	
	int slen = sizeof(server);
	// send first packet to server. sendto will auto bind
	bzero(msg, sizeof(msg));
	strcpy(msg, "First UDP Packet");
	err	= sendto(my_socket,	msg, strlen(msg),	0, (struct sockaddr	*) &server, slen);
	
	if (err < 0) { 
		printf("sendto error (UDP)"); exit(0); } else {
	}  */

	int slen = sizeof(local);

	if (bind(my_socket, (struct sockaddr*) &local, sizeof(local)) < 0) { perror("Error Binding"); exit(1); } 
	printf("UDP Bound to %s:%d\n", inet_ntoa(local.sin_addr), ntohs(local.sin_port));



    char *token;
   
	/* until the all-received flag has been set, recv data from socket */
	while(1) {

		bzero(msg,sizeof(msg));
		int recv_len = recvfrom(my_socket, msg, sizeof(msg), 0, (struct sockaddr *) &local, &slen);
		if(recv_len < 0) { printf("receive failed (UDP)"); exit(0); }
		
		/* print received data */
	
		// strip ID and data from incomming message and save to Save_Queue
	//	printf("recv:%s\n",msg);
	
		char id[10]; // char array to hold id before use of atoi
		char *c = msg; // point c to beginning of msg
		
	/*	int i = 1; // index to get data
		while(*c != ',') {
			*c++; // increase pointer
			i++; // increase index
		}
		*c++; // one more pointer position to point to beginning of data */
		int i = 0;
		while(*c != ',') {
			id[i] = *c;
			*c++;
			i++;
		}

		*c++; // one more pointer position to point to beginning of data */
		//strcat(id,"\0");
		id[i] = 0;

	//	strncpy(id,msg,i); // copy id from msg buffer
		//char *M = malloc( sizeof(char) * ( i + strlen(c)) );

		Add = (struct Message *) malloc (sizeof (struct Message)); // create temp struct
		Add->data = malloc( sizeof(char) * recv_len-(i+1) );
		Add->ID = atoi(id);
		printf("id %s\n",id);
		memcpy(Add->data, c, recv_len);
		Add->next = NULL;
		Add->data_len = recv_len-(i+1);
//		printf("Data Length:%d\n",Add->data_len);
		recv_buffer_add(Add);
		

	}


	printf("Closing UDP Socket\n");
	close(my_socket);	
}

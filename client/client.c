
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

// int udp_port, num_packets, filesize;

pthread_t tcp, udp;

int tcp_state = 0;
int udp_state = 0;
int state;

int tcp_port;
int udp_port = 10000;

char *server_address;

sem_t recv_buffer;

int all_received;


int main() {

	int rc;
	long t;
	char input[100];

	all_received = 0;

	sem_init(&recv_buffer, 0, 1);

	state = 0; // start state
	tcp_port = 8080;
//	server_address = "127.0.0.1";

	rc = pthread_create(&tcp, NULL, TCP, (void*)t);
	pthread_detach(tcp);

	if (rc) { printf("Error creating tcp thread\n"); exit(0); } 

 /*   rc = pthread_create(&udp, NULL, UDP, (void*)t);
	pthread_detach(udp);

	if (rc) {
		printf("Error creating udp thread\n");
		exit(0);	
	} 

	state = 1; */

	



	while(1) {

	}



}


#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h> // sleep()
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h> // strcpy, strcmp
#include <math.h>
#include <semaphore.h>
#include "server.h"

int udp_port;

pthread_t udp;
char client_ip[16];



void udp_connect(int port, char ip[]) {
    /* set parameters and than indicate that the UDP thread should proceed */
    udp_port = port;
    printf("udp_connect received port %d\n", port);
    strcpy(client_ip, ip);
    char *pos;
    if ((pos=strchr(client_ip, '\n')) != NULL) {
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


    int err;
    char my_buf[1024];
    int slen = sizeof(client);
    
    // fill in data for self    
  /*  server.sin_family = AF_INET ;
    server.sin_port = htons(10000) ;
    server.sin_addr.s_addr = htonl(INADDR_ANY);  // accept connection from any address */
    client.sin_family = AF_INET;
    client.sin_port = htons(udp_port);
    client.sin_addr.s_addr = inet_addr(client_ip);
    printf("UDP thread starting on port: %d\n", udp_port);

    /* setup socket and bind */
    int my_socket = socket (AF_INET, SOCK_DGRAM, 0);
  
    err = bind(my_socket, (struct sockaddr *) &client, sizeof(client));

  /*  if (err == -1) { perror ("BIND PROBLEM"); exit(0);} 

 
    int recv_len = recvfrom(my_socket, my_buf, sizeof(my_buf), 0, (struct sockaddr *) &client, &slen);
    if (recv_len == -1) { printf("recvfrom error"); exit(0); } */

    /* print initial message from client 
    printf("(UDP) Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    printf("Data: %s\n", my_buf); */

    /* at this point, a client has sent data, so we may begin sending file*/
    printf("Sending data to client...\n");

        // loop send_next(my_socket) and use a semaphore to protect the queue

        while(1) {
            send_next(my_socket);
        }
        



    /* flag an all-sent, so that our TCP server can tell client */
 /*     all_sent = 1;

    while(!all_received) {
        /* idle for now */
    

    printf("Closing UDP Socket\n");
    close(my_socket);
    pthread_exit(NULL);
}
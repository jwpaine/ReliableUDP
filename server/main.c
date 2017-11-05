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

 pthread_t tcp, udp, mh;

 sem_t mutex;
 sem_t window;

 int packetsize, filesize, number_packets;

 int ack_window;

 // struct sockaddr_in server, client;

 int *acks; // store acks so add/lookup is O(1)
 int *timer; // store time for each packet sent, so add/lookup is O(1)

 int send_base;
 int window_size;

 char filename[50];
 int udp_port;

 
int main() {

   
//  all_sent = 0;
//  all_received = 0;

    int rc;
    long t;

    // initialize mutual exclusion lock
    sem_init(&mutex, 1, 1);
    sem_init(&window, 1, 1); // allow only mutual exclusion to ack_window variable
    

//  tcp_state = 0; // wait for tcp client to connect
//  udp_state = 0; // wait for udp client to connect

 /*   rc = pthread_create(&mh, NULL, Message_Handler, (void*)t);
    if (rc) {
        printf("Error creating message handler thread\n");  
    } */

    
    rc = pthread_create(&tcp, NULL, TCP, (void*)t);
    pthread_detach(tcp);

    if (rc) {
        printf("Error creating tcp thread\n");  
    }

 /*   rc = pthread_create(&udp, NULL, UDP, (void*)t);
    pthread_detach(udp);

    if (rc) {
        printf("Error creating udp thread\n");  
    }  */

    
    while(1) {

    }

return 0;

}

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

sem_t mutex;
sem_t window;

 pthread_t mh;


void *TCP(void *p) {

    int port;
    int rc;
    long t;
    char message[50];
    int server_sock, comm_sock; // comm_sock socket used for communicating to/from client
    struct sockaddr_in my_addr, client_addr;

    printf("Enter TCP Port for self: ");
    scanf("%d", &port);
    

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = htons(INADDR_ANY); // accept connection from any address
//  inet_pton(AF_INET, "192.168.0.5" , &(my_addr.sin_addr)) ;

    /* make sure we can bind socket to ip/port */
    if (bind(server_sock, (struct sockaddr*) &my_addr, sizeof(my_addr)) < 0) { perror("Error Binding"); exit(1); } 
        
    printf("TCP Server Bound to %s:%d\n", inet_ntoa(my_addr.sin_addr), ntohs(my_addr.sin_port));
    
    /* start listening for an incomming connection (max 1 connection) from client */
    listen(server_sock, 1); 
    int len = sizeof(my_addr);
    comm_sock = accept(server_sock, (struct sockaddr *)&client_addr, &len); // accept a connection. we will read/write to comm_sock
    printf("Accepted TCP Client: %s\n", inet_ntoa(client_addr.sin_addr));

   

    int tcp_state = 0;

    /* start receiving ACKs/NAKs and 'ack' on them! :) */
   while(1) {

      bzero(message,sizeof(message)); // clear message buffer
      recv(comm_sock, &message, sizeof(message), 0); // receive message from comm_sock
      char *c = message;

    
        /* wait for client to give UDP port number and filename */
        if (*c == 'C') { 
          *c++;
          /* setup connection information */
          char* token = strtok(c, ",");
          udp_port = atoi(token);
          token = strtok(NULL, ",");
          strcpy(filename,token);

          printf("Port given by client %d\n", udp_port);
          printf("Filename requested %s\n", filename);

          rc = pthread_create(&mh, NULL, Message_Handler, (void*)t);
          if (rc) {
              printf("Error creating message handler thread\n");  
          }

          while(!ready()) {
            /* wait for msg_handler to become ready */
          }
          printf("sending client info regarding file requested\n");
          /* send client info regarding file requested */
          bzero(message,sizeof(message)); 
          /* assign max number of packets */
          strcpy(message,"F");
          char n[20];
          sprintf(n, "%d", number_packets);
          strcat(message,n);
          /* assign filesize */
          bzero(n,sizeof(n));
          sprintf(n, ",%d", filesize);
          strcat(message,n);
          printf("sending: %s\n", message);
          send(comm_sock, &message, sizeof(message), 0);  

         

          /* wait for OK status from client */
        } else if (*c == 'O') { 
              /* drop into ACK state */ 
              printf("Received OK from client\n");
              tcp_state = 2; 
              udp_connect(udp_port, inet_ntoa(client_addr.sin_addr));

          /* If the window moves and there are untransmitted packets with sequence 
            numbers that now fall within the window, these packets are transmitted. */
          } else if (*c == 'A') { // if ACK
                *c++;
                printf("ACK %d\n",atoi(c));
                // marks the packet as having been received, provided it is in the window
                 acks[atoi(c)] = 1;
                 timer[atoi(c)] = 0;
                /* If the packet's sequence number is equal to sendbase, the window base is moved forward to the unacknowledged 
            packet with the smallest sequence number. */
                 if (atoi(c) == send_base) {
                    int i = send_base;
                    while(acks[i] != 0) {
                      i++; // increase send_base until we hit an unknownledged packet
                    }
                  send_base = i;
                  printf("tcp updating send_base: %d\n", send_base);
                }
              }
            }

    printf("closing TCP socket");
    close(server_sock); // close socket
    pthread_exit(NULL);         
  

}
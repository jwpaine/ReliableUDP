
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

// struct ACK_Queue *tail,*head,*temp;

volatile int global;
sem_t recv_buffer;

int mystrlen(char *s)
{
    int i =0;
    while (*s++) i++;
    return i;
}

void *TCP(void *p) {


    struct sockaddr_in server;
    struct sockaddr_in sa_loc;

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // Local
 /*   memset(&sa_loc, 0, sizeof(struct sockaddr_in));
    sa_loc.sin_family = AF_INET;
 //   sa_loc.sin_port = htons(10000);
    sa_loc.sin_addr.s_addr = inet_addr("127.0.0.1");

    int ret = bind(sock, (struct sockaddr *)&sa_loc, sizeof(struct sockaddr));
   // assert(ret != -1); */

    int tcp_port;
    char server_address[16]; // = "127.0.0.1";
    char message[256];
    char filename[50];

    int num_packets;
    int filesize;

	printf("Client tcp thread initalized\n");

	

    printf("Enter Server IP: ");
    fgets(server_address, sizeof(server_address), stdin);
    printf("Enter Server TCP Port: ");
    scanf("%d", &tcp_port);
    getchar();
    printf("Enter Server Filename: ");
    fgets(filename, sizeof(filename), stdin);


    server.sin_family=AF_INET;
    server.sin_port=htons(tcp_port);
    sleep(1);
    inet_pton (AF_INET, server_address, &(server.sin_addr)); // setup address information for server struct
    if (connect(sock,(struct sockaddr *)&server,sizeof(server)) < 0){ // instantiate connection with server
        perror("client connect failed. Error");
        exit(0);
    } 

    /* tell server we want to setup connection information 'C' with UDP port and filename */
    bzero(message, sizeof(message));
    strcpy(message,"C");
    strcat(message,",");
    char pt[8];
    sprintf(pt, "%d", udp_port);
 //   strcat(pt, "\0");
    strcat(message,pt);
    strcat(message,",");
    strcat(message,filename);
    char *pos;
    if ((pos=strchr(message, '\n')) != NULL) {
     *pos = '\0';
    }
    printf("Sending Server Port and Filename\n");
    send(sock, &message, strlen(message), 0); 


    /* wait for server to give an "F" for max packet size, and file size --------------------------------------*/
    while(1) {
        bzero(message, sizeof(message));
        recv(sock, message, sizeof(message), 0); // receive message from comm_sock
        char *c = message;

        if (*c == 'F') { 
              
        //    *c++;
            printf("Received: %s\n",message);
            /* get file and packet information */
            char* token = strtok(&message[1], ",");

            for (int i = 0; i < 2; i++ ) {
                if (i == 0) { // port
                //  udp_port = atoi(token);
                num_packets = atoi(token);
            } else if (i == 1) { // packet size
                filesize = atoi(token);
                } 
                token = strtok(NULL, ",");
            }

            printf("Server: Number of Packets: %d Filesize: %d\n", num_packets, filesize);
            udp_start(udp_port, num_packets, filesize, server_address);
            break;
        }
    }

    sleep(1);
    /* let server know that we are ready to accept */
    printf("Telling server that we are ready for data\n");
    bzero(message, sizeof(message));
    strcpy(message,"O");
    send(sock, &message, strlen(message), 0);

    
        /* open file for writing */
    FILE *fp;
    fp = fopen("output.txt", "ab"); 
    if(fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

     int expected = 1; // counter for packet ID that we are expecting, first packet = 1
     int next = 1;
     /* read data off of recv_buffer */
     while(1) { // while expected is less than MAX_PACKETS (9 as an example here) while(expected < 9)

       
        /* if we are not expecting anymore packets, terminate!!  this will be replaced with all-sent/received messages later */
        if (expected > num_packets) { 
            break;
        } 

        sem_wait(&recv_buffer); // wait on recv_buffer

        if (Head != NULL) { // if head is not empty

     /*      if (Head->ID % 2 == 0) {
                printf("Test sleep...\n");
                sleep(1);
            }  */

            if (Head->ID == expected) { // if packet is what we are looking for
                // write Head to disk
                printf("Writing Packet %d to disk\n",Head->ID );
                fwrite(Head->data, 1, Head->data_len, fp);

                // send ACK
            //    sleep(3);
                bzero(message, sizeof(message)); // clear message buffer
                strcat(message,"A");
                sprintf(&message[1], "%d", Head->ID); // copy integer head->ID into char message
                strcat(message,"\0");
                send(sock, &message, strlen(message), 0); // send ACK to server
                printf("ACK %d\n",Head->ID);

                // update next as id that should arrive after this packet ID that just showed up
                next = Head->ID+1;


                /* do we have packets waiting in cache that may be written? Write those to disk now, and get next expected value */
                if (Cached != NULL) {
                    int e = write_cached_subsequence(expected, fp);
                
               //     printf("e=%d\n",e);
                    if (e > 0) { // if we could use a sequence of the cache, update expected value
                        expected = e;
                    } else {
                        expected++;
                    } 

                } else {
                    expected++;
                }
                    /* remove largest sub-sequence [c ... c_n] from Cached starting with c = expected++, and set expected to (c_n)++ */
            /*    } else {
                    expected++;
                } */

                // move head down by one
                // we may need free() head!!!!
                
                Temp = Head->next; // preserve next
                free(Head); // free memory for head
                Head = Temp;
                 

            } else {

                // item is not expected
                
                /* send ACK */
                bzero(message, sizeof(message)); // clear message buffer
                strcat(message,"A");
                sprintf(&message[1], "%d", Head->ID); // copy integer head->ID into char message
                strcat(message,"\0");
                send(sock, &message, strlen(message), 0); // send ACK to server
                printf("ACK %d\n",Head->ID);


                /* save message to cache for later */
                Temp = Head->next; // save everything after Head to temp
                Head->next = NULL; // clip Head from list

                if (Head->ID < expected) {
                    printf("Already received\n");
                } else {
                     cached_add(Head); // add to cache
                }
               

                Head = Temp; // set head as head->next originally

            }


        } // end if head != NULL


        sem_post(&recv_buffer);
    }







        fclose(fp);








/* until we have received an 'all-sent' and determined if all has been received, wait for TCP data */

/*	while(1) { 

		bzero(message, sizeof(message));
    	recv(sock, message, sizeof(message), 0); // receive message from comm_sock
    	printf("(TCP): %s\n", message);

    	if(strcmp(message,"all-sent")==0) {
    		sleep(1);
    	//	printf("Sending 'all-received to TCP server\n");
    		/* check if all-received (assume it has) */
    		/* send server an 'all-received' message 
           

    		bzero(message, sizeof(message));
    		strcpy(message, "all-received"); // copy message to message buffer
    		send(sock, &message, sizeof(message), 0); // send message to server, using socket sock 

    		
    		}

    	}  */
    
    printf("Closing TCP socket\n");
    close(sock);	
    pthread_exit(NULL);



}

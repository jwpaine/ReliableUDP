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
#include <time.h>
#include "server.h"







sem_t mutex;
sem_t window;




int max_sent = 0;
int last_ack = 0;
int max_timer = 100;

int r = 0;

int ready() {
    return r;
}

void send_next(int socket) { // send next message from queue

        int sent = 0;

        if (MQhead != NULL ) {
            /* send off message at the head of the message queue */
        //  printf("Data length: %d\n", MQhead->data_len);

    //        while(1) { // wait to be able to send
                
    //            if (ack_window > 0) {

                  sem_wait(&mutex);

                  char id[11]; // string to represent number and comma
                  sprintf(id, "%d,", MQhead->id); // convert integer count to string and save to char array 'id'
                  
                  /* If the sequence number is within the sender's window, the data is packetized and sent */
                  if (atoi(id) < send_base + window_size+1) {

                    printf("msg id: %d base+window: %d\n", atoi(id), send_base+window_size);

                    char *message = malloc( sizeof(char) * MQhead->data_len + strlen(id));
                    memcpy(message,id,strlen(id));
                    char *c;
                    c = &message[strlen(id)]; // point c to position after comma
                    memcpy(c, MQhead->data, MQhead->data_len); // copy data
              /*      printf("Sending %d",MQhead->id);
                    printf(" To address %s", inet_ntoa(client.sin_addr));
                    printf(" On Port %d\n", ntohs(client.sin_port)); */
                    int err = sendto(socket, message, MQhead->data_len + strlen(id), 0, (struct sockaddr*) &client, sizeof(client));
                    if (err < 0) { printf("sendto error"); perror("error"); exit(0) ; }

          //          printf("Sent message: %s\n", message);

            //        sent_list_add(MQtemp);
           //         printf("copying message to new sent list\n");

                    memcpy(&sent_list[atoi(id)],MQhead,sizeof(MQhead));
            //        printf("checking id copied... %d\n", sent_list[atoi(id)].id);
                    sent_list[atoi(id)].timer = clock();
                    timer[atoi(id)] = clock();
                   
                    

                    /* preserve head (message sent) */
                    MQtemp = MQhead;
                    /* move head down by one so next time send_next is called we will have a new message */
                    MQhead = MQhead->next_message;
                    /* pass to MQtemp to send_queue_add() // messages in the MQSent linked list can be freed or re-sent, later */
             //       MQtemp->next_message = NULL; 
                    /* add time stamp to timer array */
                 //   timer[atoi(id)] = clock(); // record time in array
           //         printf("Freeing last message off msg_queue\n");
              //      free(&MQtemp);

                 } else {
          //          printf("Scanning for timed out packets within window\n");
             //     packet id not less than window size: scan for timed out messages
                    for (int i = send_base; i < send_base + window_size; i++) {
       //         printf("Checking timer array for expired messages\n");
                //      printf("packet: %d timer: %d\n", i, timer[i]);
                      if (acks[i] == 0 && timer[i]) {
                        int elapsed = ( 1000*(clock() - timer[i]) / CLOCKS_PER_SEC );
                 //       printf("timer value for packet %d: %d\n", i, timer[i]);
                        if ( elapsed > max_timer ) {
                //          printf("elapsed time for packet %d: %d\n", i, elapsed);
                //          printf("Need to re-send message %d\n", i);
                          timer[i] = 0;
                          // resend message by adding it to the beginning of the msg_queue
                    //      printf("attempting to add packet %d back onto queue\n", i);
                          MQtemp = &sent_list[i];
                          if (MQhead == NULL) { // if there isn't a message in place on the queue
                  //          printf("only packet at head\n");
                            MQhead = MQtemp;
                            MQtail = MQtemp;
                          } else {
                   //         printf("NOT only packet at head\n");
                      //      MQtemp = MQhead;
                       //     memcpy(&sent_list[i].next_message, MQtemp, sizeof(MQhead));
                            MQtemp = MQhead;
                            sent_list[i].next_message = MQtemp;
                            MQhead = &sent_list[i];

                  //          printf("Testing head...id is now %d\n", MQhead->id);
                       //     memcpy(, MQhead, sizeof(MQhead));
                        //    memcpy(MQhead, &sent_list[i], sizeof(MQhead));
                     //       memcpy(MQhead, &sent_list[i], sizeof(MQhead) );
                  //          MQhead = &sent_list[i];
                          }
                        }
                      } 
                    }
                  } // end else (scan for timed out packets)

                   sem_post(&mutex);

                }

     
   
  }

void sent_list_add(struct Message_Queue *Add) {

    /* add message to send_list in ascending order of message id */

   

    position = MQsent;

    if (MQsent == NULL) {
        MQsent = Add;
//          printf("Linked %d at head of sent list\n", Add->id);
      
    } else {

        while(position->next_message != NULL) {
        //    printf("Moving down sent list\n");
            position = position->next_message;
        }

        position->next_message = Add; // link at end
 //        printf("Linked %d at end of sent list\n", Add->id);

    }

    

  

} // end send_cached_add(...)
void sent_list_remove(int id) {

    /*
   * Visit each node, maintaining a pointer to
   * the previous node we just visited.
   */
  for (position = MQsent;
    position != NULL;
    previous = position, position = position->next_message) {

    if (position->id == id) {  /* Found it. */
      if (previous == NULL) {
        /* Fix beginning pointer. */
        MQsent = position->next_message;
      } else {
        /*
         * Fix previous node's next to
         * skip over the removed node.
         */
        previous->next_message = position->next_message;
      }

      /* Deallocate the node. */
      printf("Freeing message %d from sent list\n",id);
      free(position);

      /* Done searching. */
      return;
    }
  }


   


}

void sent_list_resend(int id) {
     /*
   * Visit each node, maintaining a pointer to
   * the previous node we just visited.
   */

  for (position = MQsent;
    position != NULL;
    previous = position, position = position->next_message) {

    if (position->id == id) {  /* Found it. */
      if (previous == NULL) {
        /* Fix beginning pointer. */
        MQsent = position->next_message;
      } else {
        /*
         * Fix previous node's next to
         * skip over the removed node.
         */
        previous->next_message = position->next_message;
      }

      /* Deallocate the node. */
      printf("Resending %d\n",id);

   //   message_queue_add(position);
      /* place at head of queue */

      if (MQhead == NULL) { // if there isn't a message in place on the queue
        MQhead = position;
        MQtail = position;
   //     printf("message added to message queue (first message)\n");

      } else {
        /* link */
        position->next_message = MQhead;
        /* update head */
        MQhead = position;
    }


          

      /* Done searching. */
      return;
    }
  //   sem_post(&mutex); // release
  }
}

void message_queue_add(struct Message_Queue *Add) {

 

 //   printf("Added message id %d\n",Add->id);

    if (MQtail == NULL) { // if there isn't a message in place on the queue
        MQhead = Add;
        MQtail = Add;
   //     printf("message added to message queue (first message)\n");

  } else {

        MQtail->next_message = Add; // link message at end of list
        MQtail = MQtail->next_message; // update tail to point to tail of list
    //    printf("message added to tail of message queue\n");
  }

  



}
void *Message_Handler(void *p) {
    /* Open the file that we wish to transfer */
        FILE *fp = fopen(filename,"rb");
        if(fp==NULL) {
            printf("File open error");
            exit(0);
        } else { printf("File opened\n"); }

        /* get file size by seeking to the end of the file */
        fseek(fp, 0L, SEEK_END);
        filesize = ftell(fp);
        number_packets = (filesize + 1024 - 1) / 1024;  // take ceiling of (filesize / 10)


        /* return to beginning of file */
        fseek(fp, 0L, SEEK_SET);

        /* initialize sent_list */
        sent_list = malloc( sizeof(sent_list) * ( number_packets )); 

    int count = 1; // variable to id each packet, packets start at 1
    send_base = 1;
    window_size = 4;
    /* setup size of window */
 //   ack_window = 30; // allow upto 10 un-acknowledged messages

    while(1) {       
            char input_buff[1024]={0}; // input buffer to save each chunk read from file
       //     char output_buff[100] = {0}; // output buffer consisting of id+data
            int nread = fread(input_buff, 1 , sizeof(input_buff), fp); // reach in 10 bytes and store to input_buff
            if (nread > 0) { // if data read has any size

           //     printf("Bytes read %d \n", nread);        
                MQtemp = (struct Message_Queue *) malloc (sizeof (struct Message_Queue)); // create temp struct
                MQtemp->id = count; // set integer id
                MQtemp->data = malloc( sizeof(char) * ( nread ));
                memcpy(MQtemp->data, input_buff, nread); // copy nreads bytes of data from input_buff to struct
                MQtemp->data_len = nread;
                sem_wait(&mutex); 
                message_queue_add(MQtemp);
                sem_post(&mutex); 

                count++; // increase packet count id
            } 

            if (nread < sizeof(input_buff))
            {
                if (feof(fp))
                    printf("End of file\n");
                  r = 1;
                  break;
                if (ferror(fp))
                    printf("Error reading\n");
                break;
            }
        }

  //      printf("msg handler now monitoring ack array\n");

        /* setup acknowledgment array */
        acks = malloc( number_packets );
        timer = malloc( sizeof(int)*number_packets );
        bzero(acks, sizeof(acks));

        
   //     int max_timer = 100; // 75 msec

 /*       while(1) {
          // monitor array for acks in increments of 10
//    // MOVE TO MSG_SEND AND ACTIVATE WHEN PACKET IS NOT IN WINDOW TO AVOID POSSIBLE DEADLOCK
         for (int i = send_base; i < send_base + window_size; i++) {
              // check if it has expired
       //         printf("Checking timer array for expired messages\n");
                if (acks[i] == 0 && timer[i]) {
                  int elapsed = ( 1000*(clock() - timer[i]) / CLOCKS_PER_SEC );
                 printf("timer value for packet %d: %d\n", i, timer[i]);
                  if ( elapsed > max_timer ) {
                    printf("elapsed time for packet %d: %d\n", i, elapsed);
                    printf("Need to re-send message %d\n", i);
                    timer[i] = 0;

                    sem_wait(&mutex); 
          //          sent_list_resend(i);

                    MQtemp = &sent_list[i];
                    if (MQhead == NULL) { // if there isn't a message in place on the queue
                      MQhead = MQtemp;
                      MQtail = MQtemp;
                    } else {
                    
                    sent_list[i].next_message = MQhead;
               
                    MQhead = &sent_list[i];
                    }




                sem_post(&mutex); // release
                  }
                } 
             }

              sleep(1);  

          }   

          */
          
         
    
  
}







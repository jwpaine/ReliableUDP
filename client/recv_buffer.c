
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

sem_t recv_buffer;

void recv_buffer_add(struct Message *Add ) {

	sem_wait(&recv_buffer); 

//	printf("Added message: %s\n",Add->data);

	if (Head == NULL) { // if there isn't a message in place on the queue
        Head = Add;
        Tail = Add;
//        printf("message added to recv_buffer (first message)\n");

  } else {

        Tail->next = Add; // link message at end of list
        Tail = Tail->next; // update tail to point to tail of list
 //       printf("message added to tail of recv_buffer\n");
  }

  	sem_post(&recv_buffer); 

} // end function Add()

int write_cached_subsequence(int n, FILE *fp) {
	/* write maximum sequential subsequence in cache, and return next expected value, -1 if no sequence available */
	int e = -1;

	while(Cached != NULL) {
		if (Cached->ID == n+1) {
			printf("Writing message %d from cache to disk\n",Cached->ID);
			fwrite(Cached->data, 1, sizeof(Cached->data), fp);
			e = Cached->ID;
		/*	Temp = Cached->next; // preserve next
			free(Cached); // free memory
			Cached = Temp; // move down linked list, per above */
			n++; 
		} else {
			break;
			}

			Cached = Cached->next; // move down cached list

		}

		e = e + 1;
		return e; 
	}

void cached_add(struct Message *Add ) {

	/* add message to cache in ascending order of id */

	position = Cached;
	int n = 0; /* so we know if we have traversed a list or not */

	printf("Adding Message %d to cache\n",Add->ID);

	while(1) {

		if (Cached == NULL) {
			Cached = Add;
//				printf("Message first in Cached\n");
			break;
		}

		/* if the PID of node we wish to add is greater than the PID of the
		 node at the current position */
		if (Add->ID > position->ID) {
	//		printf("add > position\n");
			/* are we at the end of the list? */
			if (position->next == NULL) {
				/* place new node at end of list */
	//				printf("placing node at end of cache list\n");
				position->next = Add;
				break;
				/* else, move down in the list */
				} else {
		//				printf("moving down list by 1\n");
					previous = position; /* record previous node */
					position = position->next;
					n = 1; /* flag a traversal */
				}
		}
		/* if the PID of the node we wish to add is less than the PID of the
		 node at the current position */
	if (Add->ID < position->ID) {
		/* have we had to traverse? */
//			printf("add < position\n");
		if (n == 1) {
			/* link in middle */
//				printf("linking in middle\n");
			previous->next = Add;
			Add->next = position;
			break;
		} else {
			/* insert at beginning of list */
//				printf("inserting at beginning of cache list\n");
			Add->next = position; /* or = ptr */
//				printf("Updating root of cache\n");
                        Cached = Add;
			break;
			}
		}

	/* if the PID of the node we wish to add is equal to the PID of the
	 node at the current position */
	} // end while


} // end cached_add(...)

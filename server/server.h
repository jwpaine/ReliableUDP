


struct Message_Queue {
    int id;
    char *message;
    char *data;
    int data_len;
    int timer;
    struct Message_Queue *next_message;
} *MQhead,*MQtail,*MQsent,*MQtemp,*position, *previous, *next, *sent_list;




void *Message_Handler(void *p);
void message_queue_add(struct Message_Queue *Add);
void sent_list_resend(int id);
void sent_list_remove(int id);
void sent_list_add(struct Message_Queue *Add);
void send_next(int socket);
void udp_connect(int port, char ip[]);
int ready();

void *TCP(void *p);
void *UDP(void *p);

struct sockaddr_in server, client;




extern int packetsize, filesize, number_packets;
extern int ack_window;
extern int *acks;
extern int *timer; // store time for each packet sent, so add/lookup is O(1)
extern int send_base;
extern int window_size;
extern int udp_port;
extern char filename[50];
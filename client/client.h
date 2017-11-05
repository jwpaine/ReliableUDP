

struct Message { 
	int ID;
	char *data;
	int data_len;
	struct Message *next;
};

struct Message *Head,*Tail,*Cached,*Add,*ptr,*position,*previous,*Temp;

void *UDP(void *p);
void *TCP(void *p);
void udp_start(int port, int n_packets, int f_size, char ip[]);
void udp_stop();
void recv_buffer_add(struct Message *Add ); // incoming messages from UDP socket
void cached_add(struct Message *Add ); // cache of messages that have arrived out of order
int write_cached_subsequence(int n, FILE *fp);

extern int all_received;
extern int filesize;
extern int packetsize;
extern int udp_port;






#define MAX_STRING_LEN 512

// Node of doubly-linked list
struct list_node {
	long key;
	void *data_head;
	void *data_tail;
	struct list_node *prev;
	struct list_node *next;
};

// Store information about each file descriptor
struct FD_data {
	char filename[MAX_STRING_LEN];
	long inode; 
};

void construct_data(struct list_node **data_head, struct list_node **data_tail);
void display(int per_process, int systemWide, int Vnodes, int composite, \
		int threshold, int output_TXT, int output_binary, int pid);


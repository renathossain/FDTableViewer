#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "design.h"

void display_fd_helper(struct list_node *data_head, struct list_node *fd_head, int mode, int pid) {
	while(fd_head != NULL) {
		struct FD_data *fd_data = fd_head -> data_head;
		if (mode == 0) printf(" %-7ld %ld\n", data_head -> key, fd_head -> key);
		if (mode == 1) printf(" %-7ld %-8ld %s\n", \
			data_head -> key, fd_head -> key, fd_data -> filename);
		if (mode == 2) printf(" %-13ld %ld\n", \
			fd_head -> key, fd_data -> inode);
		if (mode == 3) printf(" %-7ld %-8ld %-15s    %ld\n", \
			data_head -> key, fd_head -> key, fd_data -> filename, \
			fd_data -> inode);
		fd_head = fd_head -> next;
	}
}

struct list_node *find_node(struct list_node *data_head, long key) {
	while(data_head != NULL) {
		if(data_head -> key == key) return data_head;
		data_head = data_head -> next;
	}
	fprintf(stderr, "Error: Entered PID value cannot be found.\n");
	return NULL;
}

void display_helper(struct list_node *data_head, int mode, int pid) {
	if(pid != -1) {
		struct list_node *pid_node = find_node(data_head, (long)pid);
		if(pid_node == NULL) return;
		display_fd_helper(pid_node, pid_node -> data_head, mode, pid);
	} else {
		while(data_head != NULL) {
			display_fd_helper(data_head, data_head -> data_head, mode, pid);
			data_head = data_head -> next;
		}
	}
}

void display_per_process(struct list_node *data_head, int pid) {
	printf("\n PID     FD \n");
	printf("============\n");
	display_helper(data_head, 0, pid);
	printf("============\n");
}

void display_systemWide(struct list_node *data_head, int pid) {
	printf("\n PID     FD       Filename \n");
	printf("========================================\n");
	display_helper(data_head, 1, pid);
	printf("========================================\n");
}

void display_Vnodes(struct list_node *data_head, int pid) {
	printf("\n FD            Inode \n");
	printf("========================================\n");
	display_helper(data_head, 2, pid);
	printf("========================================\n");
}

void display_composite(struct list_node *data_head, int pid) {
	printf("\n PID     FD       Filename           Inode \n");
	printf("===========================================\n");
	display_helper(data_head, 3, pid);
	printf("===========================================\n");
}

void display_threshold(struct list_node *data_head, int threshold) {
	printf("\n## Offending processes:\n");
	int first = 1;
	while(data_head != NULL) {
		struct list_node *fd_head = data_head -> data_head;
		int number_of_fds = 0;
		while(fd_head != NULL) {
			number_of_fds++;
			fd_head = fd_head -> next;
		}
		if (number_of_fds > threshold) {
			if (first) printf("%ld (%d)", \
				data_head -> key, number_of_fds), first = 0;
			else printf(", %ld (%d)", data_head -> key, number_of_fds);
		}
		data_head = data_head -> next;
	}
	printf("\n");
}

void display(int per_process, int systemWide, int Vnodes, int composite, \
		int threshold, int output_TXT, int output_binary, int pid) {
	struct list_node *data_head = NULL;
	struct list_node *data_tail = NULL;
	construct_data(&data_head, &data_tail);
	printf("\033c\033[1H");
	if (per_process) display_per_process(data_head, pid);
	if (systemWide) display_systemWide(data_head, pid);
	if (Vnodes) display_Vnodes(data_head, pid);
	if (composite) display_composite(data_head, pid);
	if (threshold != -1) display_threshold(data_head, threshold);
}


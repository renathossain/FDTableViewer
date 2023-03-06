#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "design.h"

// output_mode: 0 - print to console, 1 - output text (ASCII), 2 - output binary
void display_fd_helper(struct list_node *data_head, struct list_node *fd_head, \
		int mode, int pid, int output_mode) {
	FILE *fp;
	char *text1 = "\n PID     FD       Filename           Inode \n";
	char *text2 = "===========================================\n";
	if(output_mode == 1) {
		fp = fopen("compositeTable.txt", "w");
		fprintf(fp, "%s%s", text1, text2);
	} else if (output_mode == 2) {
		fp = fopen("compositeTable.bin", "wb");
		fprintf(fp, "%s%s", text1, text2);
	} else {
		fp = stdout; // print to console if output_mode is 0
	}

	while(fd_head != NULL) {
		struct FD_data *fd_data = fd_head -> data_head;
		if (mode == 0) fprintf(fp, " %-7ld %ld\n", data_head -> key, fd_head -> key);
		if (mode == 1) fprintf(fp, " %-7ld %-8ld %s\n", \
			data_head -> key, fd_head -> key, fd_data -> filename);
		if (mode == 2) fprintf(fp, " %-13ld %ld\n", \
			fd_head -> key, fd_data -> inode);
		if (mode == 3) fprintf(fp, " %-7ld %-8ld %-15s    %ld\n", \
			data_head -> key, fd_head -> key, fd_data -> filename, \
			fd_data -> inode);
		fd_head = fd_head -> next;
	}

	if(output_mode != 0) {
		fprintf(fp, "%s", text2);
		fclose(fp); // close the file if not writing to console
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

void display_helper(struct list_node *data_head, int mode, int pid, int output_mode) {
	if(pid != -1) {
		struct list_node *pid_node = find_node(data_head, (long)pid);
		if(pid_node == NULL) return;
		display_fd_helper(pid_node, pid_node -> data_head, mode, pid, output_mode);
	} else {
		while(data_head != NULL) {
			display_fd_helper(data_head, data_head -> data_head, mode, pid, output_mode);
			data_head = data_head -> next;
		}
	}
}

void display_per_process(struct list_node *data_head, int pid) {
	printf("\n PID     FD \n");
	printf("============\n");
	display_helper(data_head, 0, pid, 0);
	printf("============\n");
}

void display_systemWide(struct list_node *data_head, int pid) {
	printf("\n PID     FD       Filename \n");
	printf("========================================\n");
	display_helper(data_head, 1, pid, 0);
	printf("========================================\n");
}

void display_Vnodes(struct list_node *data_head, int pid) {
	printf("\n FD            Inode \n");
	printf("========================================\n");
	display_helper(data_head, 2, pid, 0);
	printf("========================================\n");
}

// output_mode: 0 - print to console, 1 - output text (ASCII), 2 - output binary
void display_composite(struct list_node *data_head, int pid, int output_mode) {
	char *text1 = "\n PID     FD       Filename           Inode \n";
	char *text2 = "===========================================\n";
	if (output_mode == 0) printf("%s%s", text1, text2);
	display_helper(data_head, 3, pid, output_mode);
	if (output_mode == 0) printf("%s", text2);
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

// displays the FD tables
void display(int per_process, int systemWide, int Vnodes, int composite, \
		int threshold, int output_TXT, int output_binary, int pid) {
	struct list_node *data_head = NULL;
	struct list_node *data_tail = NULL;
	construct_data(&data_head, &data_tail);
	if (per_process) display_per_process(data_head, pid);
	if (systemWide) display_systemWide(data_head, pid);
	if (Vnodes) display_Vnodes(data_head, pid);
	if (composite) display_composite(data_head, pid, 0);
	if (threshold != -1) display_threshold(data_head, threshold);
	if (output_TXT) display_composite(data_head, pid, 1);
	if (output_binary) display_composite(data_head, pid, 2);
}


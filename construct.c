#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "design.h"

// Creates a new node of doubly-linked list 
struct list_node *create_new_node(long key, void *data_head, void *data_tail) {
	struct list_node *new_node = (struct list_node *) malloc(sizeof(struct list_node));
	if (new_node == NULL) return NULL;
	new_node -> key = key;
	new_node -> data_head = data_head;
	new_node -> data_tail = data_tail;
	new_node -> prev = NULL;
	new_node -> next = NULL;
	return new_node;
}

// Performs the insertion of new_node at the tail of the list with O(1) complexity
void insert_at_tail(struct list_node **head, struct list_node **tail, struct list_node *new_node) {
	if (new_node == NULL) return;
	if (*head == NULL && *tail == NULL) {
		*head = new_node;
		*tail = new_node;
		return;
	}
	if (*head == *tail) {
		*tail = new_node;
		(*head) -> next = *tail;
		(*tail) -> prev = *head;
		return;
	}
	(*tail) -> next = new_node;
	new_node -> prev = *tail;
	*tail = new_node;
}

// Construct list of file descriptors associated with the process
void construct_fd_list(long pid, struct list_node **data_head, struct list_node **data_tail) {
	char fd_path[MAX_STRING_LEN];
	snprintf(fd_path, sizeof(fd_path), "/proc/%ld/fd", pid);
	
	DIR *dir = opendir(fd_path);
	if (dir == NULL) {
		perror("opendir");
		return;
	}
	
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		char path[MAX_STRING_LEN];
		snprintf(path, sizeof(path), "/proc/%ld/fd/%s", pid, entry->d_name);
		
		char link[MAX_STRING_LEN]; ssize_t len;
		len = readlink(path, link, sizeof(link) - 1);
		if (len == -1) continue;
		link[len] = '\0';
		
		struct stat sb;
		if (stat(path, &sb) == -1) {
			fprintf(stderr, "Error stat-ing file descriptor %s\n", fd_path);
			continue;
		}
		
		//Store data
		struct FD_data *fd_data = (struct FD_data *) malloc(sizeof(struct FD_data));
		if (fd_data == NULL) return;
		strcpy(fd_data -> filename, link);
		fd_data -> inode = sb.st_ino;
		insert_at_tail(data_head, data_tail, \
			create_new_node(strtol(entry->d_name, NULL, 10), fd_data, NULL));
	}
	closedir(dir);
}

// Construct list of all user processes
void construct_data(struct list_node **data_head, struct list_node **data_tail) {
	DIR *proc_dir = opendir("/proc");
	if (proc_dir == NULL) {
		perror("Failed to open /proc");
		return;	
	}

	struct dirent *entry;
	while ((entry = readdir(proc_dir)) != NULL) {
		if (entry->d_type != DT_DIR) continue;

		char path[MAX_STRING_LEN];
		sprintf(path, "/proc/%s/status", entry->d_name);
		struct stat stat_buf;
		if (stat(path, &stat_buf) != 0) continue;

		uid_t uid = stat_buf.st_uid;
		if (uid == geteuid()) {
			// Store data
			struct list_node *fd_head = NULL;
			struct list_node *fd_tail = NULL;
			construct_fd_list(strtol(entry->d_name, NULL, 10), &fd_head, &fd_tail);
			insert_at_tail(data_head, data_tail, \
				create_new_node(strtol(entry->d_name, NULL, 10), fd_head, fd_tail));
		}
	}
	closedir(proc_dir);
}


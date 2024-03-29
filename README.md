# FDTableViewer
Tool to display the tables used by the OS to keep track of open files, assignation of File Descriptors (FD) and processess.
  * Process FD table
  * System-wide FD Table
  * Vnodes FD table
  * Composed view of the previous tables

## How to use the program
  * To compile the code, run `make` in the terminal, and then run `make clean` to clean up the object files if needed.
  * Type `./showFDtables` to run the program in the terminal.
  * Use `--per-process`, `--systemWide`, `--Vnodes`, `--composite` to display the appropriate tables. If none of these command line arguments are given, then the program displays all of the tables
  * Enter the PID number of a process to see information only relating to that process, e.g. `./showFDtables 1001`
  * Use `--threshold=N` to show number of processes that have more than N files open
  * Use `--output_TXT` and/or `--output_binary` to output to files in text or binary format respectively.

## How did I solve the problem
  * Used the 3rd-party libraries: "dirent.h, sys/stat.h, sys/types, unistd.h" to generate the data necessary.
  * readlink to read the filename, stat for the inobe number, readdir and while loop to loop through the directories
  * Used nested linked lists to store the data, a list of user processes, and then for each process a list of file descriptors associated with that process
  * Then read the appropriate data from the data structure, and displayed it using fprintf to the console or written to a file
  * Written modular code over 3 separate files:
    * showFDtables.c for main function and argument parsing
    * construct.c for constructing the data structure used to hold all the information about the FD table
    * display.c for reading the data stored in the data structure to display the appropriate tables to the console or write them to a file

## Overview of functions
### showFDtables.c
Contains the main function and argument parsing
  * `int is_number(char *number)` - Checks whether all characters in the string are 0-9
  * `int arg_checker(int *flag_value, int default_value, int nondef_value, char *passed_string, char *true_string)` - Checks whether the passed_string matches the true_string command line argument and assigns correct value to flag_value
  * `int value_arg(int *flag_value, int default_value, char *passed_string)` - Checks whther passed_string is a number and assigns correct value to flag_value
  * `int arg_with_value_checker(int *flag_value, int default_value, char *passed_string, char *true_string_prefix)` - Checks whether passed_string matches true_string_prefix and assigns correct mumerical value to flag_value, used for '--threshold=N'
  * `int main(int argc, char **argv)` - Parses the command line arguments and makes the correct display call
### construct.c
Constructs the data structure used to hold all the information about the FD table
  * `struct list_node *create_new_node(long key, void *data_head, void *data_tail)` -  Creates a new node of doubly-linked list
  * `void insert_at_tail(struct list_node **head, struct list_node **tail, struct list_node *new_node)` - Performs the insertion of new_node at the tail of the list with O(1) complexity.
  * `void construct_fd_list(long pid, struct list_node **data_head, struct list_node **data_tail)` - Construct list of file descriptors associated with the process
  * `void construct_data(struct list_node **data_head, struct list_node **data_tail)` - Construct list of all user processes
### display.c
Reads the data stored in the data structure to display the appropriate tables to the console or write them to a file
  * `void display_fd_helper(struct list_node *data_head, struct list_node *fd_head, int mode, int pid, int output_mode)` - Helper function to output the table to console or write it to a file
  * `struct list_node *find_node(struct list_node *data_head, long key)` - Find the node with specific PID
  * `void display_helper(struct list_node *data_head, int mode, int pid, int output_mode)` - Helper function to display the table
  * `void display_per_process(struct list_node *data_head, int pid)` - Displays the per-process table
  * `void display_systemWide(struct list_node *data_head, int pid)` - Displays the SystemWide table
  * `void display_Vnodes(struct list_node *data_head, int pid)` - Displays the Vnodes table 
  * `void display_composite(struct list_node *data_head, int pid, int output_mode)` - Displays the composite table
  * `void display_threshold(struct list_node *data_head, int threshold)` - Displays the threshold
  * `void display(int per_process, int systemWide, int Vnodes, int composite, int threshold, int output_TXT, int output_binary, int pid)` - displays the FD tables

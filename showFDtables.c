#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "design.h"

// Checks whether all characters in the string are 0-9
int is_number(char *number) {
	while(*number != '\0') {
		if(*number < 48 || *number > 57) return 0;
		number++;
	}
	return 1;
}

int arg_checker(int *flag_value, int default_value, int nondef_value, char *passed_string, char *true_string) {
	if(strcmp(passed_string, true_string) == 0) {
        	if((*flag_value) != default_value) {
			fprintf(stderr, "Error: '%s' option specified multiple times.\n", true_string);
			return 0;
		}
		*flag_value = nondef_value;
		return 1;
	}
	return 0;
}

int value_arg(int *flag_value, int default_value, char *passed_string) {
	int return_value = 0;
	if(is_number(passed_string)) {
		int value = atoi(passed_string);
		if(strlen(passed_string) <= 9) *flag_value = value, return_value = 1;
		else fprintf(stderr, "Error: Entered number out of bounds.\n");
	} 
	else fprintf(stderr, "Error: Entered number contains non-digit characters.\n");
	return return_value;
}

int arg_with_value_checker(int *flag_value, int default_value, char *passed_string, char *true_string_prefix) {
	int prefix_length = strlen(true_string_prefix);
	char passed_prefix[prefix_length + 1];
	strncpy(passed_prefix, passed_string, prefix_length);
	passed_prefix[prefix_length] = '\0';
	if(arg_checker(flag_value, default_value, default_value, passed_prefix, true_string_prefix)) {
		if(value_arg(flag_value, default_value, passed_string + prefix_length)) return 1;
		else {
			fprintf(stderr, "Error: Invalid number for '%s'.\n", true_string_prefix);
			return 0;
		}
	}
	return 0;
}

int main(int argc, char **argv) {
	// Arguments
	int per_process = 0, systemWide = 0, Vnodes = 0, composite = 0;
	int threshold = -1, output_TXT = 0, output_binary = 0, pid = -1;
	
	// Parse arguments
	for(int i = 1; i < argc; i++) {
		if(arg_checker(&per_process, 0, 1, argv[i], "--per-process"));
		else if(arg_checker(&systemWide, 0, 1, argv[i], "--systemWide"));
		else if(arg_checker(&Vnodes, 0, 1, argv[i], "--Vnodes"));
		else if(arg_checker(&composite, 0, 1, argv[i], "--composite"));
		else if(arg_with_value_checker(&threshold, -1, argv[i], "--threshold="));
		else if(arg_checker(&output_TXT, 0, 1, argv[i], "--output_TXT"));
		else if(arg_checker(&output_binary, 0, 1, argv[i], "--output_binary"));
		else if(pid == -1 && is_number(argv[i]) && value_arg(&pid, -1, argv[i]));	
		else {
			fprintf(stderr, "Error: Invalid and/or duplicate argument(s).\n");
			return 1;
		}
	}

	// Default state
	if(!(per_process || systemWide || Vnodes || composite)) {
		per_process = 1, systemWide = 1, Vnodes = 1, composite = 1;
	}
	if(threshold == -1) threshold = 10;
	
	// Display the table
	display(per_process, systemWide, Vnodes, composite, threshold, output_TXT, output_binary, pid);
	return 0;
}


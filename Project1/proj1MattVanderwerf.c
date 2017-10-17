#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 80 /* The maximum length command */

char **tokenize(char *line);
void execute(char *raw_in);

int main(void) {
	char *args[MAX_LINE/2 + 1]; /* command line arguments */
	int should_run = 1; /* flag to determine when to exit program */

	while (should_run) {
		printf("osh>");
		fflush(stdout);	
	
		/**
		* After reading user input, the steps are:
		* (1) fork a child process using fork()
		* (2) the child process will invoke execvp()
		* (3) if command included &, parent will invoke wait()
		*/

		size_t input_buffer_size = 80 * sizeof(char);
		char raw_input[input_buffer_size];
		fgets(raw_input, input_buffer_size, stdin);
		raw_input[input_buffer_size - 1] = '\0';

		//debug input
		//printf("%s", raw_input);
		
		if(strcmp(raw_input, "exit") == 0) {
			should_run = 0;
			exit(0);
		} else {
			execute(raw_input);
		}
	}
	return 0;
}

char **tokenize(char *line){
	char *token;
	char **tokens = malloc(MAX_LINE * sizeof(char*));
	int index = 0;

	if(tokens == NULL) {
		printf("Token array could not be allocated to the heap");
		return NULL;
	}

	// Parse Multicommands
	if(strchr(line, ';') != NULL){
		for (char *token = strtok(line, ';'); token != NULL; token = strtok(NULL, ';')){		
			tokens[index] = token;
			index++;
		}

		if(index >= MAX_LINE)
		
	} else { // Individual Command
		for (char *token = strtok(line, ' '); token != NULL; token = strtok(NULL, ' ')){
			tokens[index] = token;
			index++;
		}
	}
}

void execute(char *raw_in){
	
}

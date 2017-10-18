#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h> //strcmp and such
#include <sys/wait.h> //waitpid

#define MAX_LINE 80 /* The maximum length command */

char **tokenize(char *line, int *is_background);
void execute(char *raw_in, int *should_run);
void update_command_history(char tokens_history[][MAX_LINE], char last_command[]);

int main(void) {
	char *args[MAX_LINE/2 + 1]; /* command line arguments */
	int should_run = 1; /* flag to determine when to exit program */
	char tokens_history[10][MAX_LINE];
	int history_count = 0;

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
		
		if(strncmp(raw_input, "exit", 4) == 0) {
			should_run = 0;
			exit(0);
		} else if(strncmp(raw_input, "history", 7) == 0) {
			// print options
			for(int i=0; i<history_count; i++) {// print command history

			}
			// scan in int
			//
			// error check
			//
			// run command
		} else {
			execute(raw_input, &should_run);
			update_command_history(tokens_history, raw_input);
			if(history_count < 10) {
				history_count++;
			}
		}
	}
	return 0;
}

char **tokenize(char *line, int *is_background){
	char *token;
	char **tokens = malloc(MAX_LINE * sizeof(char*));
	int index = 0;

	if(tokens == NULL) {
		printf("Token array could not be allocated to the heap");
		return NULL;
	}


	// Parse Multicommands
	if(strchr(line, '>') != NULL || strchr(line, '<') != NULL) { // don't allow pipes to files
		printf("Writing to file is not allowed\n");
	} else if(strchr(line, ';') != NULL){
		char *token = strtok(line, ";\n\r");
		while(token != NULL) {
			tokens[index] = token;
			index++;
			token = strtok(NULL, ";\n\r");
		}

		if(index >= MAX_LINE){
			printf("Input too large\n");
		}
		
	} else if(strchr(line, '|') != NULL) { // handle pipes
		// don't allow background pipes
		if(strchr(line, '&') != NULL){
			printf("Pipes are not allowed in the background\n");
		}

	} else { // Individual Command
		char *token =  strtok(line, " \n\r");
		while(token != NULL){
			tokens[index] = token;
			printf("%s\n", tokens[index]);
			index++;
			token = strtok(NULL, " \n\r");
		}

		if(index >= MAX_LINE){
			printf("Input too large\n");
		}// in case they have some massive input
		
		// & is last character of last token (even if there is a space before if making it its own token
		if(*(tokens[index - 1] + strlen( tokens[index - 1]) - sizeof(char)) == '&') {
			//printf("& found\n");
			*is_background = 1;
		}

		tokens[index] = NULL;
	}

	return tokens;
}

void execute(char *raw_in, int *should_run){
	//int totalTokens = 0;
	int is_background = 0;
	char **tokens = tokenize(raw_in, &is_background);
	//printf("%d", is_background);
	//check if null, send error
		
	if(!is_background) {// non background task
		int child_pid = fork();
		if(child_pid == 0){// child process
			//printf("Executing %s", tokens[0]);// debugging line
			execvp(tokens[0], tokens);
			printf("An error occured during execution, check your command format\n");
			//*should_run = 0; //exit loop?
		} else {// parent process
			int status = -1;
			waitpid(child_pid, &status, 0);
			printf("%s exited with: %d\n", tokens[0], WEXITSTATUS(status));
		}

	} else { // background task
		int child_spawner_pid = fork();
		if(child_spawner_pid == 0) {// inside child_spawner_process
			int background_process_pid = fork();
			if(background_process_pid == 0) {
				//printf("Executing %s", tokens[0]);// debugging line
				execvp(tokens[0], tokens);
				printf("An error occured during execution, check your command format\n");
				//*should_run = 0;
			} else {
				int status = -1;
				waitpid(background_process_pid, &status, 0);
				printf("%s exited with: %d\n", tokens[0], WEXITSTATUS(status));
			}
	
		}

	}

	// run it	
}

void update_command_history(char tokens_history[][MAX_LINE], char *last_command) {
	
}







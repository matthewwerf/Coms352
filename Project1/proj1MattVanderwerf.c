#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h> //strcmp and such
#include <sys/wait.h> //waitpid
#include <signal.h> //SIGTERM

#define MAX_LINE 80 /* The maximum length command */

char **tokenize(char *line, int *is_background, int *is_history, char command_history[10][MAX_LINE], int *commands_in_history, int *should_run);
void execute(char **tokens, int *should_run, int *is_background);
void update_command_history(char command_history[10][MAX_LINE], char raw_input[MAX_LINE], int *commands_in_history);
void print_history(char command_history[10][MAX_LINE], int *commands_in_history);


int main(void) {
	char *args[MAX_LINE/2 + 1]; /* command line arguments */
	int should_run = 1; /* flag to determine when to exit program */

	int commands_in_history = 0;
	char command_history[10][MAX_LINE];


	// Main while loop
	while (should_run) {
		printf("osh>");
		fflush(stdout);	

		size_t input_buffer_size = 80 * sizeof(char);
		char raw_input[MAX_LINE];
		fgets(raw_input, input_buffer_size, stdin);

		//debug input
		//printf("%s", raw_input);

		char *commands = strtok(raw_input, ";"); // split for multiple command inputs
		while(commands) {
			int is_background = 0;
			int is_history = 0;

			char **tokens = (char **)malloc(MAX_LINE * sizeof(char *));
			if(tokens == NULL) {
				printf("Tokens space could not be allocated\n");
				exit(0);
			}
			
			tokens = tokenize(commands, &is_background, &is_history, command_history, &commands_in_history, &should_run);

			if(tokens != NULL) { // Make sure valid command was returned
				execute(tokens, &should_run, &is_background);
			}

			commands = strtok(NULL, ";");

		}
	}

	return 0;
}

// Function to tokenize user input
char **tokenize(char *line, int *is_background, int *is_history, char command_history[10][MAX_LINE], int *commands_in_history, int *should_run) {
	char *token;
	char **tokens = (char **)malloc(MAX_LINE * sizeof(char*));
	int index = 0;

	if(tokens == NULL) {
		printf("Token array could not be allocated to the heap\n");
		return NULL;
	}

	if((char)line[0] != '!') {// Update command history if the input isn't a bang command
		update_command_history(command_history, line, commands_in_history);
	}
	char *line_copy = line; // is this necessary?

	token = strtok_r(line_copy, " \r\n\t\a", &line_copy);

	while(token) {

		if(strncmp(token, "exit", 4) == 0) {// Check for exit
			*should_run = 0;
		} else if(token[0] == '!') {// Check for history select command
			if(strlen(token) > 1) {
				if(token[1] == '!') { // last command
					*is_history = 0;
					if(*commands_in_history < 1) { // nothing in history yet
						printf("No commands in the history\n");
						return tokens;
					} 
					update_command_history(command_history, "history", commands_in_history); // !! is history command after history has been checked
					print_history(command_history, commands_in_history);
					//return tokenize(command_history[0], is_background, is_history, command_history, commands_in_history, should_run);
				
				} else { // num command
					*is_history = 0;
					int command_num = atoi(token+1);
					if(command_num - 1 >= *commands_in_history) { // out of bounds
						printf("Invalid command number\n");
					} else { // valid command number
						update_command_history(command_history, command_history[command_num + 1], commands_in_history);
						return tokenize(command_history[command_num + 1], is_background, is_history, command_history, commands_in_history, should_run);
					}
					return tokens;
				}
			} else {
				printf("Make sure to specify which command from history\n");
			}
		} else if(strcmp(token, "history") == 0) { // check for history command itself
			*is_history = 1;
			print_history(command_history, commands_in_history);
			//tokens[index] = token;
		} else if(strcmp(token, "&") == 0){
			*is_background = 1;
		} else {
			tokens[index] = token;
		}
		if(index < MAX_LINE - 1) {// Check for overflow
			index++;
			token = strtok_r(line_copy, " \r\n\t\a", &line_copy);
		} else {
			printf("Command size exceeded\n");
		}
	}
	tokens[index] = 0;
	return tokens;
}


// Executes tokenized input in foreground or background
void execute(char **tokens, int *should_run, int *is_background) {
	int child_pid = fork();
	
	if (*is_background == 1) {// Run in background
		if(child_pid == 0) {
			int status = execvp(tokens[0], tokens);
			if(status == -1) {// Kill if there is an error
				printf("Command does not exist\n");
				exit(0);
			}
		}
		printf("[%d]\n", child_pid);
	} else {// Wait for child to complete
		if(child_pid == 0) {
			int status = execvp(tokens[0], tokens);
			if(status == -1) {// Kill if there is an error
				printf("Command does not exist\n");
				exit(0);
			}
		}
		waitpid(child_pid, NULL, 0);
	}
}

// Shift history through array, only keep most recent 10
void update_command_history(char command_history[10][MAX_LINE], char raw_input[MAX_LINE], int *commands_in_history) {
	for(int i = 9; i > 0; i--){
	 	strcpy(command_history[i], command_history[i-1]);
	}
	strcpy(command_history[0], raw_input);
	if(*commands_in_history < 10) {
		*commands_in_history = *commands_in_history + 1;
	}
}

// Loop through history and print
void print_history(char command_history[10][MAX_LINE], int *commands_in_history) {
	for(int p = *commands_in_history-1; p >= 0; p--) {
		printf("%d: %s\n", p+1, command_history[p]);
	}
}


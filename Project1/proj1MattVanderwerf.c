#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h> //strcmp and such
#include <sys/wait.h> //waitpid
#include <signal.h> //SIGTERM

#define MAX_LINE 80 /* The maximum length command */

struct execution {
	char **tokens;
	int num_tokens;
};

char **tokenize(char *line, int *is_background, int *is_history, struct execution **command_history, int *commands_in_history, int *should_run);
void execute(char **tokens, int *should_run, int *is_background);
void update_command_history(struct execution **command_history, char **tokens, int num_commands, int *commands_in_history);
void print_history(struct execution **command_history, int *commands_in_history);


int main(void) {
	char *args[MAX_LINE/2 + 1]; /* command line arguments */
	int should_run = 1; /* flag to determine when to exit program */

	int commands_in_history = 0;
	struct execution **command_history = (struct execution **)malloc(10 * sizeof(struct execution *));
	if(command_history == NULL) {
		printf("Command History could not be allocated\n");
		exit(-1);
	}
	for(int i=0; i<10; i++) {
		command_history[i] = (struct execution *)malloc(sizeof(struct execution *));
		if(command_history[i] == NULL) {
			printf("Command History Index could not be allocated\n");
			exit(-1);
		}
	}

	while (should_run) {
		printf("osh>");
		fflush(stdout);	

		size_t input_buffer_size = 80 * sizeof(char);
		char raw_input[input_buffer_size];
		fgets(raw_input, input_buffer_size, stdin);
		raw_input[input_buffer_size - 1] = '\0';

		//debug input
		//printf("%s", raw_input);

		char *commands = strtok(raw_input, ";");
		while(commands) { // != NULL?
			int is_background = 0;
			int is_history = 0;

			char **tokens = (char **)malloc(MAX_LINE * sizeof(char *));
			if(tokens == NULL) {
				printf("Tokens space could not be allocated\n");
				exit(0);
			}
			
			tokens = tokenize(commands, &is_background, &is_history, command_history, &commands_in_history, &should_run);

			if(tokens != NULL) { // Make sure valid command was returned
				if(is_history) { // assuming history cannot be run in background because the user needs to see history before choosing
					print_history(command_history, &commands_in_history);
				} else {
					execute(tokens, &should_run, &is_background);
				}
			}

			commands = strtok(NULL, ";");



		}
	}

	for(int l = 0; l < 10; l++) {
		free(command_history[l]);
	}
	free(command_history);

	return 0;
}


char **tokenize(char *line, int *is_background, int *is_history, struct execution **command_history, int *commands_in_history, int *should_run) {
	char *token;
	char **tokens = (char **)malloc(MAX_LINE * sizeof(char*));
	int index = 0;

	if(tokens == NULL) {
		printf("Token array could not be allocated to the heap");
		return NULL;
	}

	//char *line_copy = line; // is this necessary?

	token = strtok_r(line, " \r\n\t\a", &line);

	if(strncmp(token, "exit", 4) == 0) {// Check for exit
		*should_run = 0;
		return NULL;
	} else if(token[0] == '!') {// Check for history select command
		if(strlen(token) > 1) {
			if(token[1] == '!') { // last command
				*is_background = 0;
				if(*commands_in_history < 1) { // nothing in history yet
					return tokens;
				} 
				update_command_history(command_history, command_history[ *commands_in_history - 1]->tokens, command_history[ *commands_in_history - 1]->num_tokens, commands_in_history);
				return command_history[*commands_in_history - 1]->tokens;
			
			} else { // num command
				*is_background = 0;
				int command_num = atoi(token+1);
				if(command_num >= *commands_in_history) { // out of bounds
					printf("Invalid command number");
					return NULL;
				} else { // valid command number
					update_command_history(command_history, command_history[command_num]->tokens, command_history[command_num]->num_tokens, commands_in_history);
					return command_history[command_num]->tokens;
				}
				return tokens;
			}
		} else {
			printf("Make sure to specify which command from history\n");
			return NULL;
		}
	} else if(strncmp(token, "history", 7) == 0) { // check for history command itself
		*is_history = 1;
		tokens[index] = "history";
		return tokens;
	}


	return tokens;
}

void execute(char **tokens, int *should_run, int *is_background) {
	int child_pid = fork();
	
	if(*is_background == 1) {// Run in background
		if(child_pid == 0) {
			int status = execvp(tokens[0], tokens);
			if(status == -1) {// Kill if there is an error
				printf("Command does not exist\n");
				kill(getpid(), SIGTERM);
			}
		}
		printf("[%d]\n", child_pid);
	} else {// Wait for child to complete
		if(child_pid == 0) {
			int status = execvp(tokens[0], tokens);
			if(status == -1) {// Kill if there is an error
				printf("Command does not exist\n");
				kill(getpid(), SIGTERM);
			}
		}
		waitpid(child_pid, NULL, 0);
	}
}

void update_command_history(struct execution **command_history, char ** tokens, int num_commands, int *commands_in_history) {
	command_history[*commands_in_history]->num_tokens = num_commands;
	command_history[*commands_in_history]->tokens = tokens;
	if(*commands_in_history < 10){
		*commands_in_history = *commands_in_history + 1;
	}
}

void print_history(struct execution **command_history, int *commands_in_history) {
	for(int i = *commands_in_history-1; i >= 0; i--) {
		printf("%d", i);
		for(int j=0; j < command_history[i]->num_tokens; j++){
			printf("%s", command_history[i]->tokens[j]);
		}
		printf("\n"); // to ensure output is flushed
	}
}


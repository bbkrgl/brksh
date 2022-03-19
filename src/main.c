#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "brkshell/struct.h"

#define LENGTH 1000

int main(int argc, char* argv[])
{
	char curr;
	char command_array[LENGTH];

	int i = 0;
	int j = 0;
	int cmd_c = 0;
	int pid;
	int child_status;
	int errcode;

	command_t* cmd;

	char* SHELL_STRING = "$----> "; // TODO: Can be read from env etc.
	char* PATH = "/bin/";

	printf("%s", SHELL_STRING);

	while ((curr = getchar()) != EOF && i < LENGTH) {
		if (curr == ' ' || (curr == '\n' && i != 0)) {
			if (cmd_c == 0) {
				if (i == 0)
					continue;

				cmd = malloc(sizeof(command_t));

				cmd->cmd_len = i;
				cmd->cmd = malloc((i + 1) * sizeof(char));

				for (j = 0; j < i; j++) {
					cmd->cmd[j] = command_array[j];
					command_array[j] = 0;
				}

				cmd->cmd[j] = '\0';

				cmd->arg_c = 1;
				cmd->args = malloc(sizeof(char*));
				cmd->args[0] = cmd->cmd;

				cmd_c++;
			} else {
				if (i == 0)
					continue;

				cmd->args = realloc(cmd->args, (cmd->arg_c + 1) * sizeof(char*));
				cmd->args[cmd->arg_c] = malloc((i + 1) * sizeof(char));

				for (j = 0; j < i; j++) {
					cmd->args[cmd->arg_c][j] = command_array[j];
					command_array[j] = 0;
				}

				cmd->args[cmd->arg_c][j] = '\0';

				cmd->arg_c++;
			}
			i = 0;
		}
		if (curr == '\n') {
			if (cmd_c == 0) {
				printf("%s", SHELL_STRING);
				continue;
			}

			cmd->args = realloc(cmd->args, (cmd->arg_c + 1) * sizeof(char*));
			cmd->args[cmd->arg_c] = NULL;

			if (!(pid = fork())) {
				printf("Executing command...\n");
				execvp(cmd->cmd, cmd->args);

				fprintf(stderr, "Something happened ;)\n");
				fprintf(stderr, "Just joking, here is the error trace:\n%s\n", strerror(errno));
				
				free(cmd->cmd);
				for (j = 1; j < cmd->arg_c; j++)
					free(cmd->args[j]);
				free(cmd->args);
				free(cmd);

				exit(0);
			} else if (pid == -1) {
				fprintf(stderr, "Cannot fork process, you're forked!\n");
			}

			wait(&child_status);

			if (WIFEXITED(child_status))
				printf("Child pid:%d exited successfully with status %d.\n", pid, WEXITSTATUS(child_status));

			i = 0;
			cmd_c = 0;
			free(cmd->cmd);
			for (j = 1; j < cmd->arg_c; j++)
				free(cmd->args[j]);
			free(cmd->args);
			free(cmd);
			
			printf("%s", SHELL_STRING);
		} else {
			command_array[i] = curr;
			i++;
		}
	}
}

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "brkshell/struct.h"
#include "brkshell/helper.h"
#include "brkshell/builtin.h"

#define LENGTH 1000

static inline void free_cmd(command_t* cmd)
{
	int j;
	for (j = 0; j < cmd->arg_c; j++)
		free(cmd->args[j]); // TODO: Check memleak
	free(cmd);
}

static inline int execute_process(command_t* cmd)
{
	int pid;

	if (!(pid = fork())) {
		printf("Executing command...\n");
		execvp(cmd->cmd, cmd->args);

		fprintf(stderr, "Something happened ;)\n");
		fprintf(stderr, "Just joking, here is the error trace:\n%s\n", strerror(errno));

		free_cmd(cmd);

		exit(errno);
	} else if (pid == -1) {
		fprintf(stderr, "Cannot fork process, you're forked!\n");
	}
	return pid;
}

int main(int argc, char* argv[])
{
	char curr;
	char command_array[LENGTH];

	int i = 0;
	char cmd_c = 0;
	int pid;
	int child_status;

	command_t* cmd;

	char* SHELL_STRING = "$----> "; // TODO: Can be read from env etc.
	char* PATH = "/bin/"; // TODO: PATH var

	printf("%s", SHELL_STRING);

	while ((curr = getchar()) != EOF && i < LENGTH) {
		if (curr == ' ' || (curr == '\n' && i != 0)) {
			if (cmd_c == 0) {
				if (i == 0)
					continue;

				cmd = malloc(sizeof(command_t));

				cmd->cmd_len = i;
				cmd->cmd = malloc((i + 1) * sizeof(char));

				eqstr_del(cmd->cmd, command_array, i);

				cmd->arg_c = 1;
				cmd->args = malloc(sizeof(char*));
				cmd->args[0] = cmd->cmd;

				cmd_c++;
			} else {
				if (i == 0)
					continue;

				cmd->args = realloc(cmd->args, (cmd->arg_c + 1) * sizeof(char*));
				cmd->args[cmd->arg_c] = malloc((i + 1) * sizeof(char));

				eqstr_del(cmd->args[cmd->arg_c], command_array, i);

				cmd->arg_c++;
			}
			i = 0;
		}
		if (curr == '\n') {
			if (cmd_c == 0) {
				printf("%s", SHELL_STRING);
				continue;
			}

			if (!strcmp(cmd->cmd, "exit")) {
				printf("Bye...\n");
				free_cmd(cmd);
				return 0;
			}

			cmd->args = realloc(cmd->args, (cmd->arg_c + 1) * sizeof(char*));
			cmd->args[cmd->arg_c] = NULL;

			pid = execute_process(cmd);

			wait(&child_status);

			if (WIFEXITED(child_status))
				printf("Child pid:%d exited successfully with status %d.\n",
						pid, WEXITSTATUS(child_status));

			i = 0;
			cmd_c = 0;	
			free_cmd(cmd);

			printf("%s", SHELL_STRING);
		} else {
			command_array[i] = curr;
			i++;
		}
	}
}

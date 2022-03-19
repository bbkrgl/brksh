#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define LENGTH 1000

typedef struct arg_t {
	char* arg;
	int arg_len;
} arg_t;

typedef struct command_t {
	char* cmd;
	int cmd_len;
	arg_t* args;
	int arg_c;
} command_t;

int main(int argc, char* argv[])
{
	char curr;
	char command_array[LENGTH];

	int i = 0;
	int j = 0;
	int cmd_c = 0;

	command_t* cmd;

	char* SHELL_STRING = "$----> "; // TODO: Can be read from env etc. 

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

				cmd->arg_c = 0;
				cmd->args = NULL;

				cmd_c++;
			} else {
				if (i == 0)
					continue;

				if (cmd->args == NULL)
					cmd->args = malloc(sizeof(arg_t));
				else
					cmd->args = realloc(cmd->args, (cmd->arg_c + 1) * sizeof(arg_t));
			

				cmd->args[cmd->arg_c].arg_len = i;
				cmd->args[cmd->arg_c].arg = malloc((i + 1) * sizeof(char));

				for (j = 0; j < i; j++) {
					cmd->args[cmd->arg_c].arg[j] = command_array[j];
					command_array[j] = 0;
				}

				cmd->args[cmd->arg_c].arg[j] = '\0';

				cmd->arg_c++;
			}
			i = 0;
		}
		if (curr == '\n') {	
			printf("%s", SHELL_STRING);
			
			if (cmd_c == 0)
				continue;
			
			/* Exec */

			i = 0;
			cmd_c = 0;
			free(cmd->cmd);
			for (j = 0; j < cmd->arg_c; j++)
				free(cmd->args[j].arg);
			free(cmd->args);
			free(cmd);
		} else {
			command_array[i] = curr;
			i++;
		}
	}
}

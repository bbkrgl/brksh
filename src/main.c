#include "brkshell/misc.h"
#include "brkshell/builtin.h"

#define LENGTH 1000

int main(int argc, char* argv[])
{
	char curr;
	char command_array[LENGTH];

	int i = 0;
	char cmd_c = 0;
	int pid;
	int child_status;
	int pipefd[2] = {STDIN_FILENO, STDOUT_FILENO};

	command_t* cmd;

	char* SHELL_STRING = "$----> "; // TODO: Can be read from env etc.
	char* PATH = "/bin/"; // TODO: PATH var

	printf("%s", SHELL_STRING);

	while ((curr = getchar()) != EOF && i < LENGTH) {
		if (curr == ' ' || (curr == '\n' && i != 0)) {
			if (i == 0)
				continue;

			if (cmd_c == 0) {
				cmd = malloc(sizeof(command_t));

				cmd->cmd_len = i;
				cmd->cmd = malloc((i + 1) * sizeof(char));

				eqstr_del(cmd->cmd, command_array, i);

				cmd->arg_c = 1;
				cmd->args = malloc(sizeof(char*));
				cmd->args[0] = cmd->cmd;

				cmd_c++;
			} else {
				cmd->args = realloc(cmd->args, (cmd->arg_c + 1) * sizeof(char*));
				cmd->args[cmd->arg_c] = malloc((i + 1) * sizeof(char));

				eqstr_del(cmd->args[cmd->arg_c], command_array, i);

				cmd->arg_c++;
			}
			i = 0;

			if (curr == ' ') // TODO: Too ugly, do sth
				continue;
		}

		if (curr == '|') {
			if (i != 0) {
				cmd->args = realloc(cmd->args, (cmd->arg_c + 1) * sizeof(char*));
				cmd->args[cmd->arg_c] = malloc((i + 1) * sizeof(char));

				eqstr_del(cmd->args[cmd->arg_c], command_array, i);

				cmd->arg_c++;
				i = 0;
			}
		
			if (!pipe(pipefd)) // Create pipe
				pid = execute_process(cmd, pipefd[0], pipefd[1]); // Make new process use the pipe
			else
				fprintf(stderr, "%s\n", strerror(errno));

			wait(&child_status);
			
			if (WIFEXITED(child_status) && pid != -1)
				printf("Child pid:%d exited successfully with status %d.\n",
						pid, WEXITSTATUS(child_status));
			
			cmd_c = 0;
			continue;
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
			} else if (!strcmp(cmd->cmd, "cd")) {
				if (cmd->arg_c > 3)
					fprintf(stderr, "Too many args for cd.\n");
				else if (cmd->arg_c == 1 && chdir(getpwuid(getuid())->pw_dir))
					fprintf(stderr, "%s\n", strerror(errno));
				else if (chdir(cmd->args[1]))
					fprintf(stderr, "%s\n", strerror(errno));
				
				i = 0;
				cmd_c = 0;
				free_cmd(cmd);

				printf("%s", SHELL_STRING);
				continue;
			}

			if (pipefd[1] != STDOUT_FILENO && close(pipefd[1]))
				fprintf(stderr, "%s\n", strerror(errno));

			pid = execute_process(cmd, pipefd[0], STDOUT_FILENO);

			wait(&child_status);
	
			if (WIFEXITED(child_status) && pid != -1)
				printf("Child pid:%d exited successfully with status %d.\n",
						pid, WEXITSTATUS(child_status));
			
			if (pipefd[0] != STDIN_FILENO && close(pipefd[0]))
				fprintf(stderr, "%s\n", strerror(errno));
			
			i = 0;
			cmd_c = 0;
			pipefd[0] = STDIN_FILENO, pipefd[1] = STDOUT_FILENO;
			free_cmd(cmd);

			printf("%s", SHELL_STRING);
		} else {
			command_array[i] = curr;
			i++;
		}
	}
}

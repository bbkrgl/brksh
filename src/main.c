#include "brkshell/misc.h"
#include "brkshell/builtin.h"

#define BUFFER_SIZE 1000

int main(int argc, char* argv[])
{
	char curr;
	char strbuffer[BUFFER_SIZE];

	int i = 0;
	char cmd_c = 0;
	int pid;
	int child_status;
	int pipefd[2] = {STDIN_FILENO, STDOUT_FILENO};
	int rdr_mode = 0; // No redirect

	command_t* cmd;

	char* cwd = getcwd(NULL, 0);
	char* PROMPT_FORMAT = "%s%s";
	char* SHELL_STRING = " $---> ";

	char* PATH = "/bin/"; // TODO: PATH and other env vars

	printf(PROMPT_FORMAT, cwd, SHELL_STRING);

	while ((curr = getchar()) != EOF && i < BUFFER_SIZE) {
		if (curr == ' ' || (curr == '\n' && i != 0)) {
			if (i == 0)
				continue;

			if (cmd_c == 0) {
				cmd = malloc(sizeof(command_t));

				cmd->cmd_len = i;
				cmd->cmd = malloc((i + 1) * sizeof(char));

				eqstr_del(cmd->cmd, strbuffer, i);

				cmd->arg_c = 1;
				cmd->args = malloc(sizeof(char*));
				cmd->args[0] = cmd->cmd;

				cmd_c++;
			} else {
				cmd->args = realloc(cmd->args, (cmd->arg_c + 1) * sizeof(char*));
				cmd->args[cmd->arg_c] = malloc((i + 1) * sizeof(char));

				eqstr_del(cmd->args[cmd->arg_c], strbuffer, i);

				cmd->arg_c++;
			}
			i = 0;

			if (curr == ' ') // TODO: Too ugly, do sth
				continue;
		}

		if (curr == '>') { // TODO: Sth like | after > will mess up things, check syntax
			if ((curr = getchar()) == '>')
				rdr_mode = 2;
			else
				rdr_mode = 1; // TODO: Other modes, writing on stderr etc.
		}

		if (curr == '|') {
			if (i != 0) {
				cmd->args = realloc(cmd->args, (cmd->arg_c + 1) * sizeof(char*));
				cmd->args[cmd->arg_c] = malloc((i + 1) * sizeof(char));

				eqstr_del(cmd->args[cmd->arg_c], strbuffer, i);

				cmd->arg_c++;
				i = 0;
			}
		
			if (!pipe(pipefd))
				pid = execute_process(cmd, pipefd[0], pipefd[1]);
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
				printf(PROMPT_FORMAT, cwd, SHELL_STRING);
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

				printf(PROMPT_FORMAT, cwd, SHELL_STRING);
				continue;
			}

			if (pipefd[1] != STDOUT_FILENO && close(pipefd[1]))
				fprintf(stderr, "%s\n", strerror(errno));

			if (rdr_mode == 1 || rdr_mode == 2) { // TODO: Don't know how to handle cmds like ... >> ... | ... , may fail
				int fd = open(cmd->args[cmd->arg_c-1], O_CREAT|O_TRUNC|O_RDWR); // TODO: Permissions

				if (fd != -1) {
					pid = execute_process(cmd, pipefd[0], pipefd[1]);
				} else {
					fprintf(stderr, "%s\n", strerror(errno));
				
					i = 0;
					cmd_c = 0;
					free_cmd(cmd);

					printf(PROMPT_FORMAT, cwd, SHELL_STRING);
					continue;
				}

				close(fd);
			} else {
				pid = execute_process(cmd, pipefd[0], STDOUT_FILENO);
			}


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

			printf(PROMPT_FORMAT, cwd, SHELL_STRING);
		} else {
			strbuffer[i] = curr;
			i++;
		}
	}

	return 0;
}

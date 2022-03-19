#ifndef __BRKSHELL__MISC__
#define __BRKSHELL__MISC__

#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef struct command_t {
	char* cmd;
	int cmd_len;
	char** args;
	int arg_c;
} command_t;

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

static inline void eqstr_del(char* str1, char* str2, int len)
{
	int j;

	for (j = 0; j < len; j++) {
		str1[j] = str2[j]; // eqstr
		str2[j] = 0; // _del
	}
	
	str1[j] = '\0';
}


#endif

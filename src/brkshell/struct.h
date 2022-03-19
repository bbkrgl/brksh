#ifndef __BRKSHELL_STRUCT__
#define __BRKSHELL_STRUCT__

typedef struct command_t {
	char* cmd;
	int cmd_len;
	char** args;
	int arg_c;
} command_t;


#endif

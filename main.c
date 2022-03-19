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
  int c = 0;
  int cmd_c = 0;
  int arg_c = 0;
  int arg_i = 0;

  command_t* cmd;

  while ((curr = getchar()) != EOF && i < LENGTH) {
    if (curr == ' ') {
      if (cmd_c == 0) {
        cmd = malloc(sizeof(command_t));

        cmd->cmd_len = i;
        cmd->cmd = malloc(i * sizeof(char));
        
        for (j = 0; j < i; j++) {
          cmd->cmd[j] = command_array[j];
          command_array[j] = 0;
        }
      } else {
        arg_c++;
      }
      i = 0;
    } else if (curr == '\n') {
      cmd->arg_c = arg_c;
      cmd->args = malloc(arg_c * sizeof(arg_t));

      for (j = 0; j < i && arg_c >= 0; j++,c++) {
        if (command_array[j] == ' ') {
          c = 0;
        }
      }

      /* Exec */
      
      i = 0;
      cmd_c = 0;
      free(cmd->cmd);
      for (j = 0; j < cmd->arg_c; j++)
        free(cmd->args->arg);
      free(cmd->args);
      free(cmd);
    } else {
      command_array[i] = curr;
      i++;
    }
  }
}

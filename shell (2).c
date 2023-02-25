#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
void lsh_loop(void);
char *lsh_read_line(void);
char **lsh_split_line(char *);
int lsh_launch(char **);
int lsh_execute(char **);

int main(int argc, char **argv)
{
lsh_loop();
return EXIT_SUCCESS;
}

void lsh_loop(void)
{
char *line=NULL;
char **args;
int status;

	do
		{
			printf("> ");
			line=lsh_read_line();
			args=lsh_split_line(line);
			status=lsh_execute(args);
			free(line);
			free(args);
		}while(status);
}

char *lsh_read_line(void)
{
	#define LSH_BUFFSIZE 1024
	int bufsize = LSH_BUFFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char)*bufsize);
	if(!buffer)
	{
		fprintf(stderr, "lsh:allocation error\n");
		exit(EXIT_FAILURE);
	}
	while(1)
	{
		char c=getchar();
		if(c == EOF || c == '\n')
		{
			buffer[position]='\0';
			return buffer;
			}//if
			else
			{
			buffer[position]=c;
			}//else
			position++;
			if(position>=bufsize)
			{
				bufsize+=LSH_BUFFSIZE;
				buffer=realloc(buffer,bufsize);
				if(!buffer)
				{
				fprintf(stderr, "lsh:allocation error\n");
				exit(EXIT_FAILURE);
				}//if	
			}//if
	}//while
}//lsh_read_line


char **lsh_split_line(char *line)
{
#define LSH_TOK_DELIM " \t\r\a\n"
#define LSH_TOK_BUFSIZE 64
int bufsize = LSH_TOK_BUFSIZE;
int position = 0;
char **tokens = malloc(sizeof(char *)*bufsize);
char *token, **tokens_backup;

if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}


int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }
/*
  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }
*/
  return lsh_launch(args);
}

int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

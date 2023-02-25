#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
void lsh_loop(void);             //Repl Loop
char *lsh_read_line(void);       //int thsi func we read line 
char **lsh_split_line(char *);   //parsing happens in this func
int lsh_launch(char **);         //execute is launched
int lsh_execute(char **);        //execution of command

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
			printf("> ");               //Shell prompt. Symbol showing shell is wainting for you to enter command
			line=lsh_read_line();
			args=lsh_split_line(line);  //Tokanize or parcing
			status=lsh_execute(args);   //execution
			free(line);                 //variable memory are fred(free command is built in c)
			free(args);
		}while(status);               //status is true if 30th line works if not then false and we give error
}

char *lsh_read_line(void)         //Read line
{
	#define LSH_BUFFSIZE 1024
	int bufsize = LSH_BUFFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char)*bufsize);  //size of char mult by bufsize
  //we are using dynamic allocation because we dont know length of user input
	if(!buffer)  //when allocation has failed
	{
		fprintf(stderr, "lsh:allocation error\n");
		exit(EXIT_FAILURE);
	}
	while(1)                       //infinite loop
	{
		char c=getchar();            //user char saved
		if(c == EOF || c == '\n')    // \n means if c is null
		{
			buffer[position]='\0';
			return buffer;
			}//if
			else
			{
			buffer[position]=c;       //char gets saved in buffer
			}//else
			position++;
			if(position>=bufsize)     //when user input is longer than expected
			{
				bufsize+=LSH_BUFFSIZE;  //we 2x the buffer size
				buffer=realloc(buffer,bufsize);  //we reallocate memory and all the string is copied in buffer
				if(!buffer)   //if reallocation failed ten malloc or realloc return null. !null is 1 so this loop will run
				{
				fprintf(stderr, "lsh:allocation error\n");
				exit(EXIT_FAILURE);
				}//if	
			}//if
	}//while
}//lsh_read_line


char **lsh_split_line(char *line)    //tokanizing or parsing input
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

  token = strtok(line, LSH_TOK_DELIM);   //takes string to be tokanized and delimiters
    //divides user input into tokens. C liberary function,
    //delimiters are set by the define line 77
    //delimiers are characters that are identifers for split of string
    //delimiters are not part of string. They are not counted
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {   //if tokens greater in no. than expected
      bufsize += LSH_TOK_BUFSIZE;  //2x buffer size of tikens
      tokens = realloc(tokens, bufsize * sizeof(char*));  //reallocation
      if (!tokens) {    //if reallocation fails
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}


int lsh_execute(char **args)   //executes commands
{
  int i;

  if (args[0] == NULL) {    //if no input is given by user and he presses enter
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
  return lsh_launch(args);   //if user gives input
}

int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();              //creates a duplicate of current process. returns child process id
  if (pid == 0) {            //child process
    // Child process
    if (execvp(args[0], args) == -1) { //execute sys call == -1 -->unsuccessful//exec is a sys call execvp is a varient
    //Will replace current process by new process
      perror("lsh");
    }
    //if line 140 is true or not execvp runns
    //execvp calls a sys call
    exit(EXIT_FAILURE);
  } else if (pid < 0) {           //successful
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);  //parent process waits till chiled process runs
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));  //runs untill status gives a exit and if it gives a signal
  }

  return 1;
}

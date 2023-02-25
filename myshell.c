//#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

void loop(void);
char *readLine(void);
char **splitLine(char *);
int launch(char **);
int execute(char **);

//Built in functions declarations
int cd(char **);
int help(char **);
int exit(char **);

int main(){
    printf("Shell Created By Abdul Basit Bhat\n");
    printf("Roll No: 102003121\n2CO5\n");
    loop();
    return EXIT_SUCCESS;
}

void loop(){
    char *line=NULL;
    char **args;
    int status;

    do{
         printf("#->");
         line=readLine();
         args=splitLine(line);
         status=execute(args);
         free(line);
         free(args);
    }while(status);
}

char *readLine(void){
    #define BUFFSize 1024
    int buffsize=BUFFSize;
    char *buffer=malloc(sizeof(char)*buffsize);
    int position=0;

    if(!buffer){
        printf("Allocation Error\n");
        exit(EXIT_FAILURE);
    }
    while(1){
        char c;
        c=getchar();
        if(c==EOF || c=='\n'){
            buffer[position]='\0';
            return buffer;
        }
        else{
            buffer[position]=c;
        }
        position++;
        if(position>=buffsize){
            buffsize=buffsize+BUFFSize;
            buffer=realloc(buffer,buffsize);
            if(!buffer){
                fprintf(stderr,"Allocation Error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **splitLine(char *line){
    #define DELIM " \n\t\a\r"
    #define BUFFSize2 64
    int buffsize = BUFFSize2;
    int position =0;
    char **tokens=malloc(sizeof(char*)*buffsize);
    char *token;

    if(!tokens){
        fprintf(stderr, "Allocation Error\n");
        exit(EXIT_FAILURE);
    }
    
    token=strtok(line,DELIM);

    while(token!=NULL){
        tokens[position]=token;
        position++;

        if(position>=buffsize){
            buffsize=buffsize+BUFFSize2;
            tokens=realloc(tokens,buffsize*sizeof(char*));
            if(!tokens){
                fprintf(stderr, "Allocation Failed\n");
                exit(EXIT_FAILURE);
            }
        }
token = strtok(NULL,DELIM);
    }
    tokens[position]=NULL;
   
  /*  for(int i=0;i<position;i++){
        printf("%d  ",&tokens[i]);
    }*/
    return tokens;
}

int execute(char **args){
     int i;
     if(args[0]==NULL){
         return 1;
     }
     for(i=0;i<num_builtins(),i++){
         if(strcmp(args[0],builtin_str[i])==0){
             return (*builtin_func[i])(args);
         }
     }
     return launch(args);
}

int launch(char **args){
    pid_t pid,wpid;
    int status;

    pid=fork();
    if(pid==0){
        if(execvp(args[0],args)==-1){
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    }
    else if(pid<0){
        perror("lsh");
    }
    else{
        do{
           wpid=waitpid(pid,&status,WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

void mypwd(){
    char mypwd[1024];
    getcwd(mypwd,sizeof(mypwd));
    printf("Present directory: %s",mypwd);

}

char *builtin_str[]={
    "cd",
    "help",
    "exit"
};

int (*builtin_func[])(char **)={
    &cd,
    &help,
    &exit
};
int num_builtins(){
    return sizeof(builtin_str)/sizeof(char *);
}

int cd(char **args){
    if(args[1]==NULL){
        fprintf(stderr,"Please give arguments fro cd");
    }
    else{
        if(chdir(args[1])!=0){
            perror("lsh");
        }
    }
    return -1;
}

int help(int **args){
    int i;
    printf("HELP BOOK:\n ");
    printf("Type program name and args\n");
    printf("Following are Built IN\n");

    for(i=0;i<num_builtins();i++){
        printf("  %s\n",builtin_str[i]);
    }
    printf("Use the man command for other programs info");
    return 1;
}

int exit(char **args){
    return 0;
}
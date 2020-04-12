#include <dirent.h> 
#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <time.h>

/*Use these colors to print colored text on the console*/

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"


/********************defines*********************/
//#define SIGHUP  1   /* Hangup the process */ 
//#define SIGINT  2   /* Interrupt the process */ 
//#define SIGQUIT 3   /* Quit the process */ 
//#define SIGILL  4   /* Illegal instruction. */ 
//#define SIGTRAP 5   /* Trace trap. */ 
//#define SIGABRT 6   /* Abort. */
#define  True 1
#define  False 0
#define MAXCOM 1000 // max number of letters to be supported 
#define MAXLIST 100 // max number of commands to be supported
// Clearing the shell using escape sequences
//\033 - ASCII escape character
//[H - move the cursor to the home position
//[J - erases the screen from the current line down to the bottom of the screen  
#define clc() printf("\033[H\033[J")
// Help command builtin 
#define openHelp() \
    puts(ANSI_COLOR_GREEN "\n***WELCOME TO EL_ABBAS SHELL HELP***"\
        "\nCopyright @ Suprotik Dey"\
        "\n-Use the shell at your own risk..."\
        "\nList of Commands supported:"\
        "\n>cd"\
        "\n>ls"\
        "\n>exit"\
        "\n>all other general commands available in UNIX shell"\
        "\n>pipe handling"\
        "\n>improper space handling")


/*******************function***********************/
// Greeting shell during startup 
void init_shell() 
{ 
    clc(); 
    printf(ANSI_COLOR_BLUE"\n\n\n\n******************"
        "************************"); 
    printf( ANSI_COLOR_GREEN"\n\n\n\t****EL_ABBAS SHELL****"); 
    printf(ANSI_COLOR_BLUE"\n\n\t-NAGA7ANA YA BASHMOHANDES BELLAH 3ALIK-"); 
    printf(ANSI_COLOR_BLUE"\n\n\n\n*******************"
               "***********************"); 
    char* username = getenv("USER"); 
    printf(ANSI_COLOR_BLUE"\n\n\n>>USER is: @%s", username); 
    printf("\n"); 
    sleep(2); 
    clc();
} 
//printing internal directory  directory 
int  printINDir(void) {

struct dirent *de;  // Pointer for directory entry 
  
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir("."); 
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf(ANSI_COLOR_RED "Could not open current directory" ); 
        return 0; 
    } 
   
    // read DIR using  readdir() 
    while ((de = readdir(dr)) != NULL) 
            printf("%s ", de->d_name); 
  
    closedir(dr); 
     }
//printing external directory 
 int printEXDir(void) { 
    char cwd[MAXCOM];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf(ANSI_COLOR_RED "mark@"ANSI_COLOR_RESET"EL_ABBAS-"ANSI_COLOR_GREEN"%s"ANSI_COLOR_RED":~$", cwd);
   } else {
       perror("getcwd() error");
       return 1;
   }
} 
// remove end of line
void remove_end_of_line(char line[MAXCOM]) {
  int index = 0;
  while (line[index] != '\n' && index < MAXCOM) {
    index++;
  }
  line[index] = '\0';
}
//function to readline 
char * readline(char str[]){
 printf(":- "ANSI_COLOR_RESET);
  char *ret = fgets(str, MAXCOM, stdin);
  remove_end_of_line(ret);

  return ret ;
}

// function for parsing command words 
void parseSpace(char* str, char** parsed) 
{ 
     
  
    for (int index = 0; index < MAXLIST; index++) { 
        parsed[index] = strsep(&str, "  "); 
  
        if (parsed[index] == NULL) 
            break; 
        if (strlen(parsed[index]) == 0) 
            index--; 
    } 
} 
 




// Function to take input 
int takeInput(char* str) 
{ 
    char* temp; 
  
    temp = readline(str); 
    if (strlen(temp) != 0) { 
       // add_history(temp); 
        strcpy(str, temp); 
        return 0; 
    } 
    else { 
        return 1; 
    } 
}
//log function
void log_F(char * parsed){

   if (parsed == "log") {
          const char* log[] = {"vi", "shell.log", NULL};
          execvp(log[0], (char**)log);
        } else
          execvp(parsed[0], parsed);
      }

// Function where the system command is executed 
void exec_Args(char** parsed) 
{ 
    // Forking a child 
    pid_t pid = fork();  
  
    if (pid == -1) { 
        printf("\nFailed forking child.."); 
        return; 
    } else if (pid == 0) { 
        if (execvp(parsed[0], parsed) < 0) { 
            printf("\nCould not execute command..\n"); 
        } exit(0);
        
      if ((strcmp(parsed,"&")==0 )|| strcmp(parsed,"cd")==0  ) {
        exit(0);
      }
      else {
      log_F(parsed);
      }}
     else { 
        // waiting for child to terminate 
        wait(NULL);  
        return; 
    } 
}
// Function where the piped system commands is executed 
void execArgsPiped(char** parsed, char** parsedpipe) 
{ 
    // 0 is read end, 1 is write end 
    int pipefd[2];  
    pid_t p1, p2; 
  
    if (pipe(pipefd) < 0) { 
        printf("\nPipe could not be initialized"); 
        return; 
    } 
    p1 = fork(); 
    if (p1 < 0) { 
        printf("\nCould not fork"); 
        return; 
    } 
  
    if (p1 == 0) { 
        // Child 1 executing.. 
        // It only needs to write at the write end 
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]); 
  
        if (execvp(parsed[0], parsed) < 0) { 
            printf("\nCould not execute command 1.."); 
            exit(0); 
        } 
    } else { 
        // Parent executing 
        p2 = fork(); 
  
        if (p2 < 0) { 
            printf("\nCould not fork"); 
            return; 
        } 
  
        // Child 2 executing.. 
        // It only needs to read at the read end 
        if (p2 == 0) { 
            close(pipefd[1]); 
            dup2(pipefd[0], STDIN_FILENO); 
            close(pipefd[0]); 
            if (execvp(parsedpipe[0], parsedpipe) < 0) { 
                printf("\nCould not execute command 2.."); 
                exit(0); 
            } 
        } else { 
            // parent executing, waiting for two children 
            wait(NULL); 
            wait(NULL); 
        } 
    } 
} 
  //terminator function
void terminator(pid_t sig_Pid) {
  int status;
  pid_t processPid;
  time_t localTime = time(NULL);
  FILE *logFile;

  switch (sig_Pid) {
    case SIGCHLD:
      logFile = fopen("./shell.log", "a+");
      while ((processPid = waitpid(-1, &status, WNOHANG)) > 0) {
        fprintf(logFile,"%s\tProcess %d exited with status %d\n",
                asctime(localtime(&localTime)), processPid, status);
      }
      fclose(logFile);
      break;

    case SIGQUIT:
      printf("\n***Goodbye***\n"); 
      exit(0);
      break;

    default:
      printf("%d", sig_Pid);
      break;
  }
}



  
// Function to execute builtin commands 
int ownCmdHandler(char** parsed) 
{ 
    int NoOfOwnCmds = 4, switchOwnArg = 0; 
    char* ListOfOwnCmds[NoOfOwnCmds]; 
    char* username; 
  
    ListOfOwnCmds[0] = "exit"; 
    ListOfOwnCmds[1] = "cd"; 
    ListOfOwnCmds[2] = "help"; 
    ListOfOwnCmds[3] = "hello";
    ListOfOwnCmds[4] = "log";
  
    for (int index = 0; index < NoOfOwnCmds; index++) { 
        if (strcmp(parsed[0], ListOfOwnCmds[index]) == 0) { 
            switchOwnArg = index + 1; 
            break; 
        } 
    } 
  
    switch (switchOwnArg) { 
    case 1: 
        terminator(SIGQUIT);
    case 2: 
        chdir(parsed[1]); 
        return 1; 
    case 3: 
        openHelp(); 
        return 1; 
    case 4: 
        username = getenv("USER"); 
        printf("\nHello %s.\nMind that this is "
            "not a place to play around."
            "\nUse help to know more..\n", 
            username); 
        return 1;
    case 5:
        log_F(parsed);
       
    default: 
    
        break; 
    } 
  
    return 0; 
}
// function for finding pipe 
int parsePipe(char* str, char** str_piped) 
{ 
    int i; 
    for (i = 0; i < 2; i++) { 
        str_piped[i] = strsep(&str, "|"); 
        if (str_piped[i] == NULL) 
            break; 
    } 
  
    if (str_piped[1] == NULL) 
        return 0; // returns zero if no pipe is found. 
    else { 
        return 1; 
    } 
}
//process on line 
int process_Line(char* str, char** parsed, char** parsedpipe) 
{ 
  
    char* strpiped[3]; 
    int piped = 0; 
  
    piped = parsePipe(str, strpiped); 
  
    if (piped) { 
        parseSpace(strpiped[0], parsed); 
        parseSpace(strpiped[1], parsedpipe); 
         
    } else { 
  
        parseSpace(str, parsed); 
    
    } 
  
    if (ownCmdHandler(parsed)) 
        return 0; 
    else
        return 1 + piped; 
} 











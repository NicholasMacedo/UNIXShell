/*
shell.c: Houses the source code for the shell's functions.
Student Name: Nicholas Macedo
Student ID: nmacedov
Student Number: 0889469
*/ 

#include "shell.h"

/* Main function of the shell. Houses main command loop. */
void startShell(void){
    char * userInput = NULL;
    char ** commandArgs = NULL;
    char ** userCommands = malloc(sizeof(char)*2048);
    int numIterations = 0;
    int commandStatus = 1;
    int  numArgs = 0;
    int * bgProcesses = malloc(sizeof(int)*2048);
    char currDir[2048];
    char homeDir[2048];
    char historyDir[2048];

    if (getcwd(homeDir,sizeof(homeDir)) == NULL) //Gets the home directory and stores it in an array.
    {
       perror("Directory retreval error.\n");
    }
      #ifdef DEBUG1
      printf("Home Dir: *%s*\n", homeDir);
      #endif

    if (getcwd(historyDir,sizeof(historyDir)) == NULL) //Gets the history file locatrion and stores it in an array.
    {
        perror("Directory retreval error.\n");
    }
    strcat(historyDir,"/.CIS3110_history");
      #ifdef DEBUG1
      printf("History Dir: *%s*\n", historyDir);
      #endif

    do{
        if (getcwd(currDir,sizeof(currDir)) != NULL) //Gets the current directory and prints it.
        {
           printf("%s> ",currDir); 
        }
        else{
            perror("Directory retreval error.\n");
        }
        userInput = getUserInput();
        userCommands[numIterations] = malloc(sizeof(char)*2048); //Gets user input and stores it in the history array.
        strcpy(userCommands[numIterations],userInput);
        numIterations ++;
        if (isEmpty(userInput) == 1) //Checks to see if line is empty. If it is it skips back to beginning.
        {
            commandStatus = 1;
            continue;
        }
          #ifdef DEBUG1
          printf("User Input: **%s**\n", userInput);
          #endif
        commandArgs = getArgs(userInput,&numArgs); //Splits up the user input into tokens inside commandArgs.
          #ifdef DEBUG1
          printf("Number of Args: %d\n", numArgs);
          #endif
        int i = 0;
        i = i;
          #ifdef DEBUG1
          while(commandArgs[i] != NULL){
              printf("Arg @ %d: **%s**\n",i,commandArgs[i]);
              i++;
          }
          #endif
        if (strcmp(commandArgs[0],"cd") == 0){ //Checks to see if command is changing dir.
            commandStatus = changeDir(commandArgs);
        } else if(strcmp(commandArgs[0],"$PATH") == 0 || 
            (strcmp(commandArgs[0],"echo") == 0 && strcmp(commandArgs[1],"$PATH") == 0)) { //Checks is command is accessing variable $PATH
            freeArgs(commandArgs);
            numArgs = 0;                   //Calling $PATH is the same as getconf PATH so pretend the user typed it in. 
            commandArgs = getArgs("getconf PATH",&numArgs);
            bgProcesses = executeCommand(commandArgs,numArgs,bgProcesses,&commandStatus);
        } else if(strcmp(commandArgs[0],"$HISTFILE") == 0 || 
            (strcmp(commandArgs[0],"echo") == 0 && strcmp(commandArgs[1],"$HISTFILE") == 0)) { //Checks is command is accessing variable $HISTFILE
            printf("%s\n", historyDir);
        } else if(strcmp(commandArgs[0],"$HOME") == 0 || 
            (strcmp(commandArgs[0],"echo") == 0 && strcmp(commandArgs[1],"$HOME") == 0)) {  //Checks is command is accessing variable $HOME
            printf("%s\n", homeDir);
        } else if(strcmp(commandArgs[0],"history") == 0) { //Checks to see if the user typed history
            addToHistFile(userCommands,historyDir);
            removeCommands(userCommands); //Adds the current history array into the file and "removes" the commands from the curr hist array.
            if (commandArgs[1] == NULL)
            {
                  #ifdef DEBUG1
                  printf("Printing all the history.\n"); //If just history is typed all commands are displayed.
                  #endif
                printHistory(historyDir,-10);
            } else if (strcmp(commandArgs[1],"-c") == 0)
            {
                userCommands[numIterations] = malloc(sizeof(char)*12);  //File is cleared if -c ios typed.
                strcpy(userCommands[numIterations],"history -c");
                numIterations ++;
                FILE* filePointer = fopen (historyDir,"w");  //Clear the file by opening it with w.
                if (filePointer == NULL)
                {
                    printf("Cannot open the history file.\n");
                    exit(1);
                }
                fclose(filePointer);
            } else{
                int numCommands = atoi(commandArgs[1]); //Coonvert the number to an int and print.
                  #ifdef DEBUG1
                  printf("%d\n",numCommands);
                  #endif
                printHistory(historyDir,numCommands);
            }
        } else if(hasRedir(commandArgs) != 0) { //Checks to see if any redirection using < or > is happening. Returns 0 if not.
            int reDir = hasRedir(commandArgs);
            if (reDir == 2){
                executeRedirOut(commandArgs,numArgs,&commandStatus); //If > is happening then 2 is returned.
            } else {
                executeRedirIn(commandArgs,numArgs,&commandStatus); //If < is happening then 1 is returned.
            }
            commandStatus = 1;
        } else{
            bgProcesses = executeCommand(commandArgs,numArgs,bgProcesses,&commandStatus); //Defaults to normal commands w/ and w/o args.
        }
        int j = 0;
        while(bgProcesses[j] != 0){
              #ifdef DEBUG2
              printf("MAIN: BG Process ID at %d: %d\n", j,bgProcesses[j]);
              #endif
            j++;
        }
        numArgs = 0;
        free(userInput);
        freeArgs(commandArgs);
    } while (commandStatus);
    addToHistFile(userCommands,historyDir); //Before exiting program saves history into file.
    free(bgProcesses);
    freeArgs(userCommands);
    userInput = NULL;
  return;
}

/* Gets the user input. */
char * getUserInput(void){
	char * input = malloc(sizeof(char)*2048);
    fgets(input,2048,stdin);
    input = trimLine(input); //Removes new line before returning string.
	return(input);
}

/* Removes newline from string. */
char * trimLine (char * userInput){
    int fileLineLen;
    fileLineLen = strlen(userInput);
    for (int i = 0; i < fileLineLen; i++) //Loops through string until finds new line.
    {
        if (userInput[i] == '\n')
        {
            userInput[i] = '\0'; //Replaces new line with null terminator.
        }
    }
    return(userInput);
}

/* Tokenizes the user string and saves it in command array. */
char ** getArgs(char * userInput, int * numArgs){
    char ** argArray = malloc(sizeof(char *)* 64);
    char * inputCpy = malloc(sizeof(char)* 2048);
    strcpy(inputCpy,userInput);
    char * currArg = NULL;
    currArg = strtok(inputCpy," ");
    int i = 0;
    while(currArg != NULL)
    {
          #ifdef DEBUG1
          printf("Found Arg: **%s**\n", currArg);
          #endif
        argArray[i] = malloc(sizeof(char)*500); //Copies command bit into command array.
        strcpy(argArray[i],currArg);
        (*numArgs) ++;
        i++;
        currArg = strtok(NULL," ");
    }
    argArray[i] = NULL;
      #ifdef DEBUG1
      i = 0;
      while(argArray[i] != NULL){
          printf("In FUNCT~Arg @ %d: **%s**\n",i,argArray[i]);
          i++;
      }
      #endif
    free(inputCpy);
    return(argArray);
}

/* Frees the arguments of a string array. */
void freeArgs(char ** commandArgs){
    int i = 0;
    while(commandArgs[i] != NULL){
        free(commandArgs[i]);
        i++;
    }
    free(commandArgs);
}

/* Houses the forking for normal commands and background processes. */
int * executeCommand(char ** commandArgs,int numArgs, int * bgProcesses,int * commandStatus){
    pid_t childpid;
    int status;
    int ks = 2;
    ks = ks; //Make the compiler shut up.
    int i = 0;
    i = i; //Make the compiler shut up.
      #ifdef DEBUG2
      printf("Funct: ID at 0: %d\n", bgProcesses[0] );  
      while(bgProcesses[i] != 0){
          printf("FoundProcess with ID %d\n", bgProcesses[i]);
          i++;
      }
      #endif
    
    if (strcmp(commandArgs[0],"exit") == 0) //Checks to see if exiting program.
    {
        int i = 0;
        while(bgProcesses[i] != 0){  //Loops through processes and kills them.
            #ifdef DEBUG2
            printf("Killing Process with ID %d\n", bgProcesses[i]);
            #endif
            ks = kill(bgProcesses[i],SIGTERM);
            #ifdef DEBUG2
            printf("Kill Status: %d\n",ks);
            #endif
            i++;
        }
        printf("Logging Out.\n");
        (*commandStatus) = 0;
        return(bgProcesses);
    }
    

    if (strcmp(commandArgs[numArgs-1], "&") == 0){ //Checks to see if want to run command in background.
          #ifdef DEBUG2
          printf("Command with & found.\n");
          #endif
        childpid = fork();
        if (childpid >= 0) //Fork Worked! :D
        {
            if (childpid == 0)
            {
                  #ifdef DEBUG2
                  printf("CHILD: I am child process %d\n", getpid());
                  printf("CHILD: My parents ID is %d\n", getppid());
                  #endif
                commandArgs[numArgs-1] = NULL;
                status = execvp(commandArgs[0],commandArgs); //Runs the command.
                  #ifdef DEBUG2
                  printf("CHILD: Exiting process due to error...\n");
                  #endif
                (*commandStatus) = 1;
                return(bgProcesses);
            } else{
                  #ifdef DEBUG2
                  printf("PARENT: I am parent process %d\n", getpid());
                  printf("PARENT: Waiting for child top exit.\n");
                  printf("PARENT: Child's exit code is %d\n", WEXITSTATUS(status));
                  #endif
                int i = 0;
                while(bgProcesses[i] != 0){
                      #ifdef DEBUG2
                      printf("bg process ID: %d\n", bgProcesses[i]); //Stores the child processes ID in an array for later killing.
                      #endif
                    i++;
                }
                bgProcesses[i] = childpid; //Save the child PID into an array of ints.
                  #ifdef DEBUG2
                  printf("Assigned Value At %d to %d\n",i, bgProcesses[i]);
                  #endif
            }
        } else {
            perror("Forking Failed. :(\n");
                exit(-1);
        }
    } else {
          #ifdef DEBUG2
          printf("Command with %d args found!\n", numArgs-1); //Defaults to running command in forground.
          #endif
        childpid = fork();
        if (childpid >= 0) //Fork Worked! :D
        {
            if (childpid == 0)
            {
                  #ifdef DEBUG2
                  printf("CHILD: I am child process %d\n", getpid());
                  printf("CHILD: My parents ID is %d\n", getppid());
                  #endif

                status = execvp(commandArgs[0],commandArgs);
                  #ifdef DEBUG2
                  printf("CHILD: Exiting process due to error...\n");
                  #endif
                exit(status);
            } else{
                  #ifdef DEBUG2
                  printf("PARENT: I am parent process %d\n", getpid());
                  printf("PARENT: Waiting for child top exit.\n");
                  #endif
                waitpid(childpid,&status,0); //Waiting for child to end.
                  #ifdef DEBUG2
                  printf("PARENT: Child's exit code is %d\n", WEXITSTATUS(status));
                  #endif
                if (WEXITSTATUS(status) > 0)
                {
                    printf("Error. Invalid Command.\n");
                }
                (*commandStatus) = 1;
                return(bgProcesses);
            }
        } else {
            perror("Forking Failed. :(\n");
                exit(-1);
        }
    }
    (*commandStatus) = 1;
    return(bgProcesses);
}

/* Changes the directory and displays error if does not exist. */
int changeDir(char ** commandArgs){
    if(chdir(commandArgs[1]) != 0){
        printf("Directory does not exist.\n");
    }
    return(1);
}

/* Adds the given commands to the history file. */
void addToHistFile(char ** userCommands,char fileName[2048]){
    int i = 0;
      #ifdef DEBUG1
      printf("File Name: **%s**\n",fileName);
      while(userCommands[i] != NULL){
          printf("User Command: **%s**\n",userCommands[i]);
          i++;
      }
      #endif
    FILE * filePointer = fopen(fileName,"a"); //Open with append to add to file with cointents.
    if (filePointer == NULL){
        printf("Cannot open the file. (Include File Location.)\n");
        exit(1);
    }

    while(userCommands[i] != NULL){
        if (strcmp(userCommands[i],"DONOTADD") != 0) //Checks for DONOTADD because that is already in file.
        {
            fprintf(filePointer,"%s\n",userCommands[i]);
        }
        i++;
    }
    fclose(filePointer);
    return;
}

/* Checks to see if line is empty. Returns 0 for no and 1 for yes. */
int isEmpty(char * userInput){
    for (int i = 0; i < strlen(userInput); ++i)
    {
        if (userInput[i] != ' ')
        {
            return(0);
        }
    }
    return(1);
}

/* 
Checks to see if command is redirecting. Returns 2 if output, 1 if input and 0 if none.
*/
int hasRedir(char ** commandArgs){
    int i = 0;
    char * currArg = NULL;
    while(commandArgs[i] != NULL){ //Loops through every command string
        currArg = commandArgs[i];
        for (int i = 0; i < strlen(currArg); ++i){ //Loops through the command string characters.
            if (currArg[i] == '>'){
                return(2);
            }
            if (currArg[i] == '<'){
                return(1);
            }
        }
        i++;
    }
    return(0);
}

/* Used when executing a redirect for output. */
void executeRedirOut(char ** commandArgs,int numArgs,int * commandStatus){
    pid_t childpid;
    int status;
        #ifdef DEBUG2
        printf("Command with %d args found!\n", numArgs-1); //Defaults to running command in forground.
        #endif
        childpid = fork();
        if (childpid >= 0) //Fork Worked! :D
        {
            if (childpid == 0)
            {
                  #ifdef DEBUG2
                  printf("CHILD: I am child process %d\n", getpid());
                  printf("CHILD: My parents ID is %d\n", getppid());
                  #endif
                FILE * filePointer;
                filePointer = freopen(commandArgs[numArgs-1],"w",stdout); //Used to redirect stdout to the given path.
                dup2(fileno(filePointer),1);
                commandArgs[numArgs-2] = NULL; //Removes everything but the command.
                numArgs = numArgs - 2;
                status = execvp(commandArgs[0],commandArgs); //Runs the command
                  #ifdef DEBUG2
                  printf("CHILD: Exiting process due to error...\n");
                  #endif
                fclose(filePointer); //Closes the file stream pointer.
                exit(status);
            } else{
                  #ifdef DEBUG2
                  printf("PARENT: I am parent process %d\n", getpid());
                  printf("PARENT: Waiting for child top exit.\n");
                  #endif
                waitpid(childpid,&status,0); //Waiting for child to end.
                  #ifdef DEBUG2
                  printf("PARENT: Child's exit code is %d\n", WEXITSTATUS(status));
                  #endif
                if (WEXITSTATUS(status) > 0)
                {
                    printf("Error. Invalid Command.\n");
                }
                (*commandStatus) = 1;
                return;
            }
        } else {
            perror("Forking Failed. :(\n");
                exit(-1);
        }
}

/* Used when executing a redirect for input. */
void executeRedirIn(char ** commandArgs,int numArgs,int * commandStatus){
    pid_t childpid;
    int status;
        #ifdef DEBUG2
        printf("Command with %d args found!\n", numArgs-1); //Defaults to running command in forground.
        #endif
        childpid = fork();
        if (childpid >= 0) //Fork Worked! :D
        {
            if (childpid == 0)
            {
                  #ifdef DEBUG2
                  printf("CHILD: I am child process %d\n", getpid());
                  printf("CHILD: My parents ID is %d\n", getppid());
                  #endif
                FILE * filePointer;
                filePointer = freopen(commandArgs[numArgs-1],"r",stdin); //Changes the read to stdin.
                commandArgs[numArgs-2] = NULL;
                numArgs = numArgs - 2;
                status = execvp(commandArgs[0],commandArgs);
                  #ifdef DEBUG2
                  printf("CHILD: Exiting process due to error...\n");
                  #endif
                fclose(filePointer);
                exit(status);
            } else{
                  #ifdef DEBUG2
                  printf("PARENT: I am parent process %d\n", getpid());
                  printf("PARENT: Waiting for child top exit.\n");
                  #endif
                waitpid(childpid,&status,0); //Waiting for child to end.
                  #ifdef DEBUG2
                  printf("PARENT: Child's exit code is %d\n", WEXITSTATUS(status));
                  #endif
                if (WEXITSTATUS(status) > 0)
                {
                    printf("Error. Invalid Command.\n");
                }
                (*commandStatus) = 1;
                return;
            }
        } else {
            perror("Forking Failed. :(\n");
                exit(-1);
        }
}

/* Prints the history to the screen via the given interval. */
void printHistory(char * historyDir,int numPrint){
    int numLines = getNumLines(historyDir);
    int lineNum = 1;
      #ifdef DEBUG1
      printf("Num Lines: %d\n Num Print: %d\n",numLines,numPrint);
      #endif
    int printAfter = 0;
    printAfter = numLines - numPrint; //Gets the ling to print after.
      #ifdef DEBUG1
      printf("Print After SUB: %d\n", printAfter);
      #endif
    FILE * filePointer;
    char * temp = malloc(sizeof(char)*2048);
    if (numPrint == -10){
        printAfter = 0;
    } else if (numPrint < 0 && numPrint != -10){  //-10 is a magic number to signify printing the entire thing.
        printf("Error. Expecting non negative number.\n");
    } else if(numPrint > numLines){
        printAfter = 0;
    }
    filePointer = fopen(historyDir,"r"); //Opens file for reading.
      #ifdef DEBUG1
      printf("Print After: %d\n", printAfter);
      #endif
    while(fgets(temp,2048,filePointer) != NULL){
        if (lineNum > printAfter)
        {
            printf(" %d  %s",lineNum,temp);  //Prints the lines that the line number is greater than the amount we want to print after.
        }
        lineNum ++;
    }
    fclose(filePointer);
    free(temp);
    return;
}

/* Gets the number of lines in the history file. */
int getNumLines(char * historyDir){
    int numLines = 0;
    FILE* filePointer;
    char * temp = malloc(sizeof(char)*2048);
    filePointer = fopen(historyDir,"r");
    while(fgets(temp,2048,filePointer) != NULL){  //Loop through file until NULL and adds to variable.
        numLines ++;
    }
    free(temp);
    fclose(filePointer);
    return(numLines);
}

/* Removes the commands that have already been added. */
void removeCommands(char ** userCommands){
    int i = 0;
    while(userCommands[i] != NULL){
        strcpy(userCommands[i],"DONOTADD"); //Changes command to DO NOT ADD.
        i++;
    }
}
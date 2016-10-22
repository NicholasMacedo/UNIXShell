/*************************shell.h****************************************
Student Name: Nicholas Macedo                  Student Number:
Date: October 5th 2016                         Course Name: CIS 3110
****************************************************************************/

#ifndef shell_H
#define shell_H

#define _POSIX_SOURCE
#include "shell.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

//#define DEBUG1 /* Used for testing everything but forking */
//#define DEBUG2 /* Used for the forking print statements */

/*
 startShell: The main function of the shell.
 Pre: Takes in nothing.
 Post: Returns nothing.
*/
void startShell(void);

/*
 getUserInput: Gets the input from the user.
 Pre: Takes in nothing.
 Post: Returns a string with the user's input and removed new line.
*/
char * getUserInput(void);

/*
 teimLine: Removes the \n from the given string.
 Pre: Takes in the string to remove the line.
 Post: Returns the string with the removed newline.
*/
char * trimLine (char * userInput);

/*
 getArgs: Tokenizes the user's input by a space and saves it in an array of strings.
 Pre: Takes in the number of arguments and the string to tokenize.
 Post: Returns array of strings.
*/
char ** getArgs(char * userInput, int * numArgs);

/*
 freeArgs: Frees the array of strings.
 Pre: Takes in name array it should free.
 Post: Returns nothing. Frees memory.
*/
void freeArgs(char ** commandArgs);

/*
 executeCommand: Runs the command given to it in the command array. Runs commands that are not special commands.
 Pre: Takes in the command array, number of arguments in the array, array of background process numbers and the status of the program.
 Post: Returns array of background ints as well as changing the command status via the passing of memory.
*/
int * executeCommand(char ** commandArgs,int numArgs, int * bgProcesses, int * commandStatus);

/*
 changeDir: Used to chgange directory when called to the one given in the command.
 Pre: Takes in name command array.
 Post: Returns int for command status.
*/
int changeDir(char ** commandArgs);

/*
 addToHistFile: Adds the commands in the array to the history file given in the file name.
 Pre: Takes in the command array to add and the file name.
 Post: Returns nothing. Fills a file with data.
*/
void addToHistFile(char ** userCommands,char fileName[2048]);

/*
 isEmpty: Checks to see if the line is empty. (Only containing a new line or spaces.)
 Pre: Takes in line to check if empty.
 Post: Returns 1 is empty and 0 if not empty.
*/
int isEmpty(char * userInput);

/*
 hasRedir: Checks to see if the command given has redirection in it.
 Pre: Takes in the command array.
 Post: Returns 2 if found outgoing, 1 if found input and 0 if does not contain.
*/
int hasRedir(char ** commandArgs);

/*
 executeRedirOut: Used to execute the code for output redirection of commands.
 Pre: Takes in the command array, the number of args and the command status.
 Post: Returns nothing. Changes the command status via memory.
*/
void executeRedirOut(char ** commandArgs,int numArgs,int * commandStatus);

/*
 executeRedirIn: Used to execute the code for input redirection of commands.
 Pre: Takes in the command array, the number of args and the command status.
 Post: Returns nothing. Changes the command status via memory.
*/
void executeRedirIn(char ** commandArgs,int numArgs,int * commandStatus);

/*
 printHistory: Used to print the history to the screen.
 Pre: Takes in the history file name and the number of lines the user would like to print.
 Post: Returns nothing.
*/
void printHistory(char * historyDir,int numLines);

/*
 getNumLines: Gets the number of lines for the printHistory function.
 Pre: Takes in the location of the history file.
 Post: Returns the number of lines.
*/
int getNumLines(char * historyDir);

/*
 removeCommands: Removes the commands from the list by changing them to say "DONOTADD".
 Pre: Takes in the command history array.
 Post: Returns nothing.
*/
void removeCommands(char ** userCommands);

#endif

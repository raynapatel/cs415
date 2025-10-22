#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "command.h"
#include "string_parser.h"

// ------------------------------ Matching Commands ------------------------------
    //match command to command.c and pass arguments in needed
    //space_commands.command_list takes in command name as first token
    //add one arg to all command checks
int process_command(command_line* space_commands) {
    char* command = space_commands->command_list[0];
    int num_tokens = space_commands->num_token;

    if (strcmp(command, "exit") == 0) {
        return 1; //set the flag
    }

    if (strcmp(command, "ls") == 0) {
        if (num_tokens == 1) { //ls takes 0 args
            listDir();
        } else {
            fprintf(stderr, "Error! Unsupported parameters for command: %s\n", command);
        }
    } 
    else if (strcmp(command, "pwd") == 0) {
        if (num_tokens == 1) {
            showCurrentDir();
        }
        else {
            fprintf(stderr, "Error! Unsupported parameters for command: %s\n", command);
        }
    }
    else if (strcmp(command, "mkdir") == 0) {
        if (num_tokens == 2) {
            makeDir(space_commands->command_list[1]);
        } else {
            fprintf(stderr, "Error! Unsupported parameters for command: %s\n", command);
        }
    }
    else if (strcmp(command, "cp") == 0) {
        if (num_tokens == 3) {
            copyFile(space_commands->command_list[1], 
            space_commands->command_list[2]);
        } else {
            fprintf(stderr, "Error! Unsupported parameters for command: %s\n", command);
        }
    }
    else if (strcmp(command, "mv") == 0) {
        if (num_tokens == 3) {
            moveFile(space_commands->command_list[1], 
            space_commands->command_list[2]);   
        } else {
            fprintf(stderr, "Error! Unsupported parameters for command: %s\n", command);
        }
    }
    else if (strcmp(command, "rm") == 0) {
        if (num_tokens == 2) {
            deleteFile(space_commands->command_list[1]);
        } else {
            fprintf(stderr, "Error! Unsupported parameters for command: %s\n", command);
        }
    }
    else if (strcmp(command, "cat") == 0) {
        if (num_tokens == 2) {
            displayFile(space_commands->command_list[1]);
        } else {
            fprintf(stderr, "Error! Unsupported parameters for command: %s\n", command);
        }
    }
    else {
        fprintf(stderr, "Error! Unrecognized command: %s\n", command);
    }
    return 0;
}


// ------------------------------ Core Program ------------------------------
// needs to be able to read, parse, and execute by reading from command.c
int main(int argc, char *argv[]) {

    //default for interactive mode input
    FILE *input_stream = stdin;
    //default for file mode
    int interactive_mode = 0;
    // ---------------------------------- INTERACTIVE MODE ----------------------------------
    if (argc == 1) {
        //set flag to interactive mode on
        interactive_mode = 1;
        //run in interactive mode
        //argv[0] --> psuedo-shell

    // ---------------------------------- FILE MODE ----------------------------------
    } else if (argc == 3 && strcmp(argv[1], "-f") == 0) {
        //run in file mode
        //argv[0] --> psuedo-shell
        //argv[1] --> "-f"
        //argv[2] --> input filename
        
        // ------------------------------ Open Input ------------------------------
        //open input file for reading
        input_stream = fopen(argv[2], "r");
        if (input_stream == NULL) {
            perror("Error opening input file");
            return 1;
        }

        // ------------------------------ Open Output ------------------------------
        //open output file for writing with open() system call
        //gives file descriptor
        int foutput = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (foutput == -1) {
            perror("Error opening output.txt");
            fclose(input_stream);
            return 1;
        }

        //redirect STDOUT (file descriptor 1) to point to output.txt
        //anything that should go to STDOUT to be redirected to output.txt
        dup2(foutput, STDOUT_FILENO);
        close(foutput);

        // ------------------------------ Error Handling ------------------------------
    } else {
        //error, invalid # of arguments
        //exit
        fprintf(stderr, "%s [-f <filename>]\n", argv[0]);
         return 1;
    }

    // ------------------------------ Unified Processing Loop ------------------------------
    char* line_buf = NULL;
    size_t line_buf_size = 0;
    //hold return value
     ssize_t line_size; 
    //flag to handle exit command
    int should_exit = 0;

    while(1) {
        if (interactive_mode) {
            printf(">>>");
        }
            
        //read input from stdin (keyboard)
        line_size = getline(&line_buf, &line_buf_size, input_stream);

        //check for error
        if (line_size < 0) {
            break;
        }

        // ------------------------------ Parsing Commands ------------------------------
        //parse into individual command strings (delimiter is semicolon)
        command_line semi_colon_commands  = str_filler(line_buf, ";");
            
        for (int i = 0; i < semi_colon_commands.num_token; i++) {
            //get single command string
            char* single_command_str = semi_colon_commands.command_list[i];
        
            //parse commands into commands and argument
            command_line space_commands = str_filler(single_command_str, " ");
                
            if (space_commands.num_token == 0) {
                //free result of space parsing
                free_command_line(&space_commands);
                continue;
            }

            if (process_command(&space_commands)) {
                //set flag for main while(1) loop
                should_exit = 1;
                //free space_commands before breaking --> prevent memory leak
                free_command_line(&space_commands);

                break;
            }
            
        // -------------------------- While Loop Cleanup --------------------------
        //free space commands
            free_command_line(&space_commands);
        } //end loop for semicolons
            
        //free result of semicolon parsing
        free_command_line(&semi_colon_commands);

        //check if flag indicates to exit main while loop
        if (should_exit) {
            break;
        }
    }

     // ------------------------------ Final Cleanup ------------------------------
    //free the buffer
    free(line_buf);
    //reset pointer for extra safety
    line_buf = NULL;
        
    if (input_stream != stdin) {
        fclose(input_stream);
    }

    printf("Bye Bye!\n");

    return 0;
}
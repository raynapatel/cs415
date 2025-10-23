Project 1: Pseudo Shell Implementation
This project involves creating a simplified shell program that can parse and execute user commands in both interactive and file mode. The shell should be capable of handling basic command execution

Directory Structure
    main.c
    command.c
    command.h
    string_parser.c
    string_parser.h
    Makefile
    input.txt
    example-output.txt
    test_script.sh
    Report_Collection_Template.docx
    project-1-description.pdf
    pseudo-shell


Compilation
To compile the project, navigate to the project1 directory and run:

    make
        This will generate the pseudo-shell executable.

Usage
To run the pseudo shell, execute:

    ./pseudo-shell
        
The shell will prompt for user input. Enter commands as you would in a typical shell.

Example
$ ./pseudo-shell
pseudo-shell> ls -l
total 12
-rw-r--r-- 1 user user  123 Apr 24 16:58 file1.txt
-rw-r--r-- 1 user user  456 Apr 24 16:58 file2.txt
pseudo-shell> exit

    Features
    Executing of standard commands (e.g., ls, pwd)
    Input/output redirection using > and <
    Basic error handling for invalid arguments and invalid commands

Testing
    A test script test_script.sh is provided to automate testing of the shell's functionalities. To run the tests:

    ./test_script.sh
        The script will execute a series of commands and compare the output against expected results.
//Purpose: 
//implement commands for the shell as specified in the project instructions

//instructions: 
//no printf or fopen
//use write() instead
//using low-level system calls

#define _GNU_SOURCE
#include "command.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>

//ls | system call --> opendir(), readdir(), closedir()
void listDir() {
    //pass in "." as current directory
    DIR* dir = opendir(".");

    //error: null is returned
    if (dir == NULL) {
        char* exist_msg = "Directory does not exist\n";
        write(1, exist_msg, strlen(exist_msg));
        return;
        }

    //declare entry variable
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        int len = strlen(entry->d_name);
        write(1, entry->d_name, len);
        write(1, " ", 1);
    }
    write(1, "\n", 1);
    closedir(dir);
}


//pwd | system call --> getcwd
void showCurrentDir() {
    //allocate a buffer on the stack
    char path_buffer[1024];

    //getcwd() asks kernal for CWD
    //puts it into the buffer
    if (getcwd(path_buffer, sizeof(path_buffer)) != NULL) {
        //successful: write path to stdout (fd 1)
        write(1, path_buffer, strlen(path_buffer));
        //newline
        write(1, "\n", 1);
    } else {
        //error: write error message to stderr (fd 2)
        char* error_msg = "Error: Could not get current directory\n";
        write(2, error_msg, strlen(error_msg));
    }
}


//mkdir | system call --> mkdir()
void makeDir(char *dirName) {
    //0755 provides r/w/execute for owner
    //and read/execute for group and others
    int status = mkdir(dirName, 0755);

    //error when return -1:
    if (status == -1) {
        switch (errno) {
            //how to handle if directory exists already
            case EEXIST:
                char* exist_msg = "Directory already exists!\n";
                write(2, exist_msg, strlen(exist_msg));
                break;
            default: 
                char* error_msg = "Error: Could not make directory\n";
                write(2, error_msg, strlen(error_msg));
                break;
        }
    }
    //if successful: nothing because no output
}


//cd | system call --> chdir()
void changeDir(char *dirName) {
    int status = chdir(dirName);

    //error when -1 is returned
    if (status == -1) {
        char* error_msg = "Error: Directory not found\n";
        write(2, error_msg, strlen(error_msg));
    }
    //if successful: nothing because no output
}


//cp | system call --> open() *  2, read(), write(), close() * 2
void copyFile(char *sourcePath, char *destinationPath) {
    int src_fd, dst_fd;
    ssize_t bytes_read;
    char buffer[1024];
    struct stat stat_buf;
    //buffer for final path
    char final_dst_path[1024];

    //check if dst is directory
    int stat_result = stat(destinationPath, &stat_buf);
    if (stat_result == 0 && S_ISDIR(stat_buf.st_mode)){
        //if dst is directory
        //need non-const copy of sourcePath for basename()
        char* src_path_copy = strdup(sourcePath);
        char* src_basename = basename(src_path_copy);

        //build new path
        strcpy(final_dst_path, destinationPath);
        strcat(final_dst_path, "/");
        strcat(final_dst_path, src_basename);

        //free strdup
        free(src_path_copy);
    } else {
        //dst is a file
        strcpy(final_dst_path, destinationPath);
    }

    //open the src file and read from it
    src_fd = open(sourcePath, O_RDONLY);
    //error when -1 is returned
    if (src_fd < 0) {
        char* error_msg = "Error: Cannot open source file\n";
        write(2, error_msg, strlen(error_msg));
        return;
    }

    //open the destination file
    dst_fd = open(final_dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    //error when -1
    if (dst_fd < 0) {
        char* error_msg = "Error: Cannot open destination file\n";
        write(2, error_msg, strlen(error_msg));
        //close source file before returning to prevent mem leaks
        close(src_fd);
        return;
    }

    //read-write loop
    //return number of bytes read
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        //write() exact number of bytes read
        ssize_t bytes_written = write(dst_fd, buffer, bytes_read);

        //check if write failed
        if (bytes_written != bytes_read) {
            char* error_msg = "Error: Failed to write to destination file\n";
            write(2, error_msg, strlen(error_msg));
            break;
        }
    }

    //close both file descriptors
    close(src_fd);
    close(dst_fd);
}


//mv | system call --> reuse functions --> copyFile(sp, dp), deleteFile(sp)
void moveFile(char *sourcePath, char *destinationPath) {
    copyFile(sourcePath, destinationPath);
    deleteFile(sourcePath);
}


//rm | system call --> unlink()
void deleteFile(char *filename) {
    if (unlink(filename) == -1) {
        char* error_msg = "File not found\n";
        write(2, error_msg, strlen(error_msg));
    }
    //successful: no output
}


//cat | system calls --> open(), read(), write(), close()
void displayFile(char *filename) {
    //open file and read only
    int fd = open(filename, O_RDONLY);
    //error if less than 0
    if (fd < 0) {
        char* error_msg = "Error: Cannot open file\n";
        write(2, error_msg, strlen(error_msg));
        //stop the function
        return;
    }

    //prepare buffer for read-write loop
    char buffer[1024];
    ssize_t bytes_read;

    //start the loop
    //read() to fill the buffer and return # of bytes read
    //returns 0 when hits end of file
    //returns -1 on error
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        //write bytes read to stdout
        write(1, buffer, bytes_read);
    }

    //close file descriptor
    close(fd);
}
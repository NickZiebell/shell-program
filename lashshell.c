#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// Define a maximum number of paths in the path list
#define MAX_PATHS 10

// Define a maximum path length
#define MAX_PATH_LENGTH 256

// Global path list and count of paths
char* pathList[MAX_PATHS];
int pathCount = 0;

// Function to print the current path list
void printPath() {
    for (int i = 0; i < pathCount; i++) {
        printf("%s\n", pathList[i]);
    }
}

// Function to add a path to the path list
void addToPath(char* newPath) {
    if (pathCount < MAX_PATHS) {
        // Insert the new path at the beginning of the list
        memmove(pathList + 1, pathList, pathCount * sizeof(char*));
        pathList[0] = strdup(newPath); // Duplicate the path
        pathCount++;
    }
}

// Function to remove a path from the path list
void removeFromPath(char* targetPath) {
    for (int i = 0; i < pathCount; i++) {
        if (strcmp(pathList[i], targetPath) == 0) {
            // Free the memory for the path
            free(pathList[i]);
            // Shift the remaining paths to the left
            for (int j = i; j < pathCount - 1; j++) {
                pathList[j] = pathList[j + 1];
            }
            pathCount--;
            return; // Path found and removed
        }
    }
    // Path not found, print an error
    fprintf(stderr, "Error: Path not found\n");
}

// Function to clear the path list
void clearPath() {
    for (int i = 0; i < pathCount; i++) {
        free(pathList[i]);
    }
    pathCount = 0;
}

int main(int argc, char **argv) {
    char *line;
    pid_t pid, wpid;
    size_t bufsize = 0;
    ssize_t characters_read;
    int status;

    while (1) {
        // Print a prompt character
        printf("$ ");

        // Read a line from the terminal
        characters_read = getline(&line, &bufsize, stdin);

        if (characters_read == -1) {
            perror("getline error");
            break;
        }

        // Trim newline character
        if (line[characters_read - 1] == '\n') {
            line[characters_read - 1] = '\0';
        }

        // Check if the entered command is "exit"
        if (strcmp(line, "exit") == 0) {
            break; // Terminate the shell
        } else if (strncmp(line, "path", 4) == 0) {
            // Check if the command starts with "path"

            // Tokenize the input to get the command and argument(s)
            char *command = strtok(line, " ");
            char *argument = strtok(NULL, " ");

            if (argument == NULL) {
                fprintf(stderr, "Error: Missing argument for path command\n");
                continue; // Continue to the next iteration
            }

            if (strcmp(argument, "print") == 0) {
                // Option (a): Print the current path variable
                printPath();
            } else if (strcmp(argument, "add") == 0) {
                // Option (b): Add a path to the path list
                char *newPath = strtok(NULL, " ");
                if (newPath == NULL) {
                    fprintf(stderr, "Error: Missing path to add\n");
                } else {
                    addToPath(newPath);
                }
            } else if (strcmp(argument, "remove") == 0) {
                // Option (c): Remove a path from the path list
                char *targetPath = strtok(NULL, " ");
                if (targetPath == NULL) {
                    fprintf(stderr, "Error: Missing path to remove\n");
                } else {
                    removeFromPath(targetPath);
                }
            } else if (strcmp(argument, "clear") == 0) {
                // Option (d): Clear the path list
                clearPath();
            } else {
                // Invalid argument for the "path" command
                fprintf(stderr, "Error: Invalid argument for path command\n");
            }
        } else if (strncmp(line, "cd", 2) == 0) {
            // Check if the command starts with "cd "
            char *newDir = line + 3; // Extract the directory path from the command

            // Check if the "cd" command has an argument
            if (strlen(newDir) == 0) {
                fprintf(stderr, "cd: missing argument\n");
            } else {
                // Attempt to change the directory using chdir()
                if (chdir(newDir) == 0) {
                    // Successfully changed directory
                    // You may want to update the prompt to reflect the new directory
                } else {
                    perror("cd"); // Print an error message if chdir fails
                }
            }
        } else {
            // Handle other commands as you were doing
            pid = fork();

            if (pid == 0) { // Child process
                // Create an argument vector for the child process using malloc
                char **argz = (char **)malloc(2 * sizeof(char *));

                if (argz == NULL) {
                    perror("malloc error");
                    exit(1); // Terminate child process on memory allocation error
                }

                // Pass the entire user-entered command as the argument
                argz[0] = line;
                argz[1] = NULL;

                // Execute the command in the child process
                execvp(argz[0], argz);

                // If execvp returns, it means there was an error
                perror("execvp error");
                free(argz); // Free allocated memory
                exit(1);    // Terminate child process
            } else if (pid > 0) { // Parent process
                // Wait for the child process to complete
                wpid = waitpid(pid, &status, 0);
                // Add error checking for waitpid if necessary
            } else { // Fork error
                perror("Fork error");
            }
        }
    }

    // Free allocated memory for line and path list
    clearPath();
    
    return 0;
}

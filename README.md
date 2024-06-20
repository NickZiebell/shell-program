# shell-program
Basic shell program capable of various commands

Built-in Commands:

exit: Terminates the shell program.
path print: Prints the current list of paths (pathList).
path add <newPath>: Adds a new path (newPath) to the pathList.
path remove <targetPath>: Removes a specific path (targetPath) from the pathList.
path clear: Clears all paths from the pathList.
cd <directory>: Changes the current working directory to <directory>.
External Commands:

Any other command entered that is not built-in (exit, path, cd) will be executed using execvp() in a child process.

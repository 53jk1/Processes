// $ gcc -Wall -Wextra forkexec.c -o forkexec
// $ ./forkexec

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define UNUSED(a) ((void)a)

int main (int argc, char **argv, char **envp) {
    UNUSED(argc);
    UNUSED(argv);

    // Make a copy of this process. From a code point of view,
    // you can distinguish between a child process and a parent
    // process by the return value of fork(). The parent process
    // will receive the child's PID, and the return value will be
    // 0 for a child process.
    pid_t pid = fork();
    if (pid == 0) {
        // Child process code.

        char *new_argv[] = {
            "/usr/bin/vim",
            NULL
        };
        execve(new_argv[0], new_argv, envp); // This feature never
                                            // comes back.

        // Unless it fails.
        perror("Failed to create process");
        return 1;
    }

    // Parent process code.
    printf("Process started (pid: %u). Waiting for it to exit.\n",
        (unsigned int)pid);
    waitpid(pid, NULL, 0);
    puts("The child process has exited.");
    return 0;
}
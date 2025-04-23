#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <signal.h>
#include <fcntl.h>
#include "LineParser.h"

short debug_mode = 0;

void halt(int pid)
{
    if (kill(pid, SIGTSTP) == -1)
    {
        perror("Failed to send signal");
    }
}

void wakeup(int pid)
{
    if (kill(pid, SIGCONT) == -1)
    {
        perror("Failed to send signal");
    }
}

void ice(int pid)
{
    if (kill(pid, SIGINT) == -1)
    {
        perror("Failed to send signal");
    }
}

int checkSignalsCmd(cmdLine *pCmdLine)
{
    if (strcmp(pCmdLine->arguments[0], "halt") == 0)
    {
        halt(atoi(pCmdLine->arguments[1]));
        return 1;
    }
    if (strcmp(pCmdLine->arguments[0], "wakeup") == 0)
    {
        wakeup(atoi(pCmdLine->arguments[1]));
        return 1;
    }
    if (strcmp(pCmdLine->arguments[0], "ice") == 0)
    {
        ice(atoi(pCmdLine->arguments[1]));
        return 1;
    }
    return 0;
}

int execute(cmdLine *pCmdLine)
{

    if (checkSignalsCmd(pCmdLine)) {
        return 1;
    }

    if (strcmp(pCmdLine->arguments[0], "quit") == 0)
    {
        return 0;
    }

    if (strcmp(pCmdLine->arguments[0], "cd") == 0)
    {
        if (chdir(pCmdLine->arguments[1]) == -1)
        {
            perror("cd failed");
        }
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) // child
    {
        if (pCmdLine->inputRedirect != NULL)
        {
            close(0);
            int fd_in = open(pCmdLine->inputRedirect, O_RDONLY);
            if (fd_in < 0)
            {
                perror("Failed to open input file");
                _exit(1);
            }
        }

        if (pCmdLine->outputRedirect != NULL)
        {
            close(1);
            int fd_out = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd_out < 0)
            {
                perror("Failed to open output file");
                _exit(1);
            }
        }

        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        perror("Command Failed");
        _exit(1);
    }
    else // parent
    {
        if (debug_mode)
        {
            fprintf(stderr, "PID: %d\nExecuting command: %s\n", pid, pCmdLine->arguments[0]);
        }
        if (pCmdLine->blocking == 1)
        {
            if (debug_mode)
            {
                fprintf(stderr, "Child is blockingm, waiting\n");
            }
            waitpid(pid, NULL, 0);
        }
    }

    return 1;
}

void runShell(void)
{
    char cwd[PATH_MAX];
    char cmdString[2048];
    cmdLine *command;

    short cont = 1;

    while (cont)
    {
        getcwd(cwd, sizeof(cwd));
        printf("%s > ", cwd);

        fgets(cmdString, sizeof(cmdString), stdin);
        command = parseCmdLines(cmdString);
        cont = execute(command);

        freeCmdLines(command);
    }
}

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "-d") == 0)
    {
        debug_mode = 1;
        printf("debug mode is on\n");
    }
    runShell();
}
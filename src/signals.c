#include "signals.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

static pid_t foreground_pid = -1;

void set_foreground_pid(pid_t pid) {
    foreground_pid = pid;
}

void handle_sigint(int sig) {
    (void)sig;
    if (foreground_pid > 0) {
        // Child handles it, we just print newline if needed
        printf("\n");
    } else {
        printf("\nCaught SIGINT. Type 'exit' to quit.\nvisionos> ");
        fflush(stdout);
    }
}

void handle_sigtstp(int sig) {
    (void)sig;
    if (foreground_pid > 0) {
        // User requested to kill on Ctrl+Z (custom behavior)
        kill(foreground_pid, SIGKILL);
        printf("\nProcess %d killed (SIGTSTP).\n", foreground_pid);
        foreground_pid = -1;
    } else {
        printf("\nCaught SIGTSTP. No process running.\nvisionos> ");
        fflush(stdout);
    }
}

void handle_sigchld(int sig) {
    (void)sig;
    int saved_errno = errno;
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        if (pid == foreground_pid) {
            foreground_pid = -1;
        }
    }
    errno = saved_errno;
}

void handle_sigalrm(int sig) {
    (void)sig;
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGKILL);
        printf("\nTimeout! Process %d killed.\n", foreground_pid);
    }
    printf("\nSession timed out due to inactivity. Bye!\n");
    exit(0);
}

void setup_signals() {
    struct sigaction sa;

    // SIGINT
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    // SIGTSTP (Ctrl+Z)
    sa.sa_handler = handle_sigtstp;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &sa, NULL);

    // SIGCHLD
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    // SIGALRM
    sa.sa_handler = handle_sigalrm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa, NULL);
}

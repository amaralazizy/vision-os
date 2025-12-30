#ifndef SIGNALS_H
#define SIGNALS_H
#include <unistd.h>

void setup_signals();
void set_foreground_pid(pid_t pid);

#endif

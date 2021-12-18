#ifndef H_TERMINAL
#define H_TERMINAL

#include <paper.h>
#include <stdbool.h>
#include <mongoose.h>

#if MG_ARCH == MG_ARCH_UNIX

#include <pty.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/ioctl.h>

struct terminal_t {
    int fd;
    pid_t pid;
    paper_t in;
    paper_t out;
};

#else /* MG_ARCH_WIN32 */

#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <winpty.h>
#include <winpty_constants.h>

struct terminal_t {
    winpty_t * pty;
    HANDLE process;
    paper_t in;
    paper_t out;
};

bool handle_agent_request(int argc, char * argv[]);

#endif

bool terminal_open(struct terminal_t * terminal);
bool terminal_set_size(struct terminal_t * terminal, int cols, int rows);
void terminal_close(struct terminal_t * terminal);

#endif /* H_TERMINAL */
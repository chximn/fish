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


typedef int terminal_id_t;

#else /* MG_ARCH_WIN32 */

#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <winpty.h>
#include <winpty_constants.h>

typedef winpty_t * terminal_id_t;

bool handle_agent_request(int argc, char * argv[]);

#endif

struct terminal_t {
    terminal_id_t id;
    paper_t in;
    paper_t out;
};

bool open_terminal(struct terminal_t * terminal);
bool set_terminal_size(struct terminal_t * terminal, int cols, int rows);

#endif /* H_TERMINAL */
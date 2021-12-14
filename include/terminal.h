#ifndef H_TERMINAL
#define H_TERMINAL

#include <paper.h>
#include <stdbool.h>
#include <mongoose.h>

#if MG_ARCH == MG_ARCH_UNIX

#include <pty.h>
#include <termios.h>
#include <stdlib.h>

#else /* MG_ARCH_WIN32 */

#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <winpty.h>
#include <winpty_constants.h>

bool handle_agent_request(int argc, char * argv[]);

#endif

bool open_terminal(paper_t * in, paper_t * out);

#endif /* H_TERMINAL */
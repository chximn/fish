#include <terminal.h>

#if MG_ARCH == MG_ARCH_UNIX

bool terminal_open(struct terminal_t * terminal) {
	int fd;
    struct termios *term = NULL;
    struct winsize *win = NULL;
	pid_t childpid = forkpty(&fd, NULL, term, win);

    if (childpid < 0) {
        return false;
    }

    else if (childpid == 0) {
        execl("/bin/sh", "/bin/sh", (char *) NULL);
        abort();
    }

    terminal->fd = fd;
    terminal->pid = childpid;
    terminal->in = (paper_t) fd;
    terminal->out = (paper_t) fd;
	return true;
}

bool terminal_set_size(struct terminal_t * terminal, int cols, int rows) {
    struct winsize ws;
    ws.ws_col = cols;
    ws.ws_row = rows;
    return ioctl(terminal->fd, TIOCSWINSZ, &ws) >= 0;
}

void terminal_close(struct terminal_t * terminal) {
    kill(terminal->pid, SIGKILL);
    close(terminal->fd);
}

#else /* MG_ARCH_WIN32 */

bool handle_agent_request(int argc, char * argv[]) {
    if (argc >= 7 && !strcmp(argv[1], "agent")) {
		int wargc = 0;
		wchar_t *cmdline = GetCommandLineW();
		wchar_t **wargv = CommandLineToArgvW(cmdline, &wargc);
		winpty_agent(wargv[2], wargv[3], wargv[4], wargv[5], wargv[6]);
        return true;
	}

    return false;
}

bool terminal_open(struct terminal_t * terminal) {

    winpty_error_ptr_t err;
    winpty_config_t * agentCfg = winpty_config_new(0, &err);
    if (agentCfg == NULL) {
        printf("1%ls\n", winpty_error_msg(err));
        return false;
    }

    wchar_t program[1024];
    GetModuleFileNameW(NULL, program, 1024);
    winpty_t * pty = winpty_open(agentCfg, &err, program, L"agent");
    if (pty == NULL) {
        printf("2%ls\n", winpty_error_msg(err));
        return false;
    }

    winpty_config_free(agentCfg);

    HANDLE conin  = CreateFileW(winpty_conin_name(pty), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    HANDLE conout = CreateFileW(winpty_conout_name(pty), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (conin == INVALID_HANDLE_VALUE) {
        printf("failed to open handle conin\n");
        return false;
    }

    if (conout == INVALID_HANDLE_VALUE) {
        printf("failed to open handle conout\n");
        return false;
    }

    winpty_spawn_config_t * spawnCfg = winpty_spawn_config_new(WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN, NULL, L"cmd", NULL, NULL, &err);
    if (spawnCfg== NULL) {
        printf("3%ls\n", winpty_error_msg(err));
        return false;
    }

    HANDLE process = NULL;
    BOOL spawnSuccess = winpty_spawn(pty, spawnCfg, &process, NULL, NULL, &err);
    if (!spawnSuccess || process == NULL) {
        printf("4%ls\n", winpty_error_msg(err));
        return false;
    }

    terminal->pty = pty;
    terminal->process = process;
    terminal->in = (paper_t) conin;
    terminal->out = (paper_t) conout;

    return true;
}

bool terminal_set_size(struct terminal_t * terminal, int cols, int rows) {
    return winpty_set_size(terminal->pty, cols, rows, NULL);
}

void terminal_close(struct terminal_t * terminal) {
    CloseHandle(terminal->process);
    CloseHandle(terminal->in);
    CloseHandle(terminal->out);
    winpty_free(terminal->pty);
}

#endif
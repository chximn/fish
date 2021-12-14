#include <paper.h>

bool paper_read(paper_t paper, void * buf, paper_size_t buf_size, paper_size_t * n_read) {
#if MG_ARCH == MG_ARCH_UNIX
    ssize_t ret = read(paper, buf, buf_size);
    if (ret < 0) {
        return false;
    }

    else {
        *n_read = ret;
        return true;
    }
#else /* MG_ARCH_WIN32 */
    return ReadFile(paper, buf, buf_size, n_read, NULL);
#endif
}

bool paper_write(paper_t paper, void * buf, paper_size_t to_write, paper_size_t * n_written) {
#if MG_ARCH == MG_ARCH_UNIX
    ssize_t ret = write(paper, buf, to_write);
    if (ret < 0) {
        return false;
    }

    else {
        *n_written = ret;
        return true;
    }
#else  /* MG_ARCH_WINDOWS */
    return WriteFile(paper, buf, to_write, n_written, NULL);
#endif
}
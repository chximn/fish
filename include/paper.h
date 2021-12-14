#ifndef H_PAPER
#define H_PAPER

#include <stdbool.h>
#include <mongoose.h>

#if MG_ARCH == MG_ARCH_UNIX

#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

typedef int paper_t;
typedef size_t paper_size_t;

#else  /* MG_ARCH_WIN32 */

#include <windows.h>

typedef HANDLE paper_t;
typedef DWORD paper_size_t;

#endif

bool paper_read(paper_t paper, void * buf, paper_size_t buf_size, paper_size_t * n_read);
bool paper_write(paper_t paper, void * buf, paper_size_t to_write, paper_size_t * n_written);


#endif /* H_PAPER */
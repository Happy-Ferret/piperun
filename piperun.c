/*
 * piperun.c - Run ELF binary code read from standard input.
 *
 * AUTHOR: Joey Pabalinas <alyptik@protonmail.com>
 * See LICENSE file for copyright and license details.
 */

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/memfd.h>

extern char **environ;

int main(int argc, char **argv)
{
	int mem_fd;
	(void)argc;

	if ((mem_fd = syscall(SYS_memfd_create, "piperun", MFD_CLOEXEC)) == -1)
		err(EXIT_FAILURE, "%s", "error creating memfd");

	for (;;) {
		ssize_t buf_len;
		size_t count = sysconf(_SC_PAGESIZE);
		unsigned char buf[count];
		if ((buf_len = read(STDIN_FILENO, buf, count)) == -1) {
			if (errno == EINTR || errno == EAGAIN)
				continue;
			err(EXIT_FAILURE, "%s", "error reading from stdin");
		}
		/* break on EOF */
		if (buf_len == 0)
			break;
		if (write(mem_fd, buf, buf_len) == -1) {
			if (errno == EINTR || errno == EAGAIN)
				continue;
			err(EXIT_FAILURE, "%s", "error writing to memfd");
		}
	}

	fexecve(mem_fd, argv, environ);
	/* fexecve() should never return */
	err(EXIT_FAILURE, "%s", "fexecve() returned");
}

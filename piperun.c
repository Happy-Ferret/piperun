/*
 * piperun.c - Run ELF binary code read from standard input.
 *
 * AUTHOR: Joey Pabalinas <alyptik@protonmail.com>
 * See LICENSE file for copyright and license details.
 */

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/memfd.h>

#define UNUSED __attribute__ ((unused))

extern char **environ;

int main(int argc UNUSED, char **argv)
{
	int fd;
	ssize_t buf_len;
	unsigned char *buf;
	/* 2MB */
	unsigned count = 1024 * 1024 * 2;

	if ((fd = syscall(SYS_memfd_create, "piperun", MFD_CLOEXEC)) == -1)
		err(EXIT_FAILURE, "%s", "error creating memfd");
	if ((buf = malloc(count)) == NULL)
		err(EXIT_FAILURE, "%s", "error allocating buffer");

	for (;;) {
		if ((buf_len = read(STDIN_FILENO, buf, count)) == -1) {
			free(buf);
			buf = NULL;
			err(EXIT_FAILURE, "%s", "error reading from stdin");
		}
		/* break on EOF */
		if (buf_len == 0)
			break;

		if (write(fd, buf, buf_len) == -1) {
			free(buf);
			buf = NULL;
			err(EXIT_FAILURE, "%s", "error writing to memfd");
		}
	}

	free(buf);
	buf = NULL;
	fexecve(fd, argv, environ);
	/* fexecve() should never return */
	err(EXIT_FAILURE, "%s", "fexecve() returned");
}

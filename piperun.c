/*
 * piperun.c - Run ELF binary code read from stdin
 *
 * AUTHOR: Joey Pabalinas <alyptik@protonmail.com>
 * See LICENSE file for copyright and license details.
 */

#include <string.h>
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
	/* 2MB */
	unsigned count = 1024 * 1024 * 2;
	unsigned char *buf = malloc(count);

	if ((fd = syscall(SYS_memfd_create, "Virtual file", MFD_CLOEXEC)) == -1) {
		fprintf(stderr, "%s\n", strerror(errno));
		free(buf);
		return 1;
	}

	for (;;) {
		if ((buf_len = read(0, buf, count)) == -1) {
			fprintf(stderr, "%s\n", strerror(errno));
			free(buf);
			return 1;
		}
		/* break on EOF */
		if (buf_len == 0)
			break;
		if (write(fd, buf, buf_len) == -1) {
			fprintf(stderr, "%s\n", strerror(errno));
			free(buf);
			return 1;
		}
	}

	free(buf);
	fexecve(fd, argv, environ);

	/* fexecve() should never return */
	fprintf(stderr, "%s\n", strerror(errno));
	return 1;
}

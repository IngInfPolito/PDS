/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <spl.h>
#include <clock.h>
#include <thread.h>
#include <current.h>
#include <syscall.h>

/*
 * Write syscall.
 *
 * Basic implementation that ignores file descriptor and writes directly to the console.
 *
 * Returns number of bytes written if successful,
 * or a negative number on error (errno is set).
 */

ssize_t sys_write(int fd, const void* buf, size_t nbytes) {

	int errno;
	int spl;
	size_t i;

	(void)fd;

	/* Check if buf is valid */
	if (!buf) {
		errno = EFAULT;
		return errno;
	}

	/* Do nothing if bytes to write are less than one */
	if (nbytes < 1) {
		return 0;
	}

	putch_prepare();

	for (i = 0; i < nbytes; i++) {
		putch(((char*)buf)[i]);
	}

	putch_complete();

	return (ssize_t)i;

}

/*
 * Read syscall.
 *
 * Basic implementation that ignores file descriptor and reads directly from the console.
 *
 * Returns number of bytes read if successful,
 * or a negative number on error (errno is set).
 */

ssize_t sys_read(int fd, const void* buf, size_t nbytes) {

	int errno;
	int spl;
	size_t i;

	(void)fd;

	/* Check if buf is valid */
	if (!buf) {
		errno = EFAULT;
		return errno;
	}

	/* Do nothing if bytes to read are less than one */
	if (nbytes < 1) {
		return 0;
	}

	for (i = 0; i < nbytes; i++) {
		((char*)buf)[i] = getch();
	}

	return (ssize_t)i;

}

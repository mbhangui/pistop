/*
 * $Log: server.c,v $
 * Revision 1.1  2020-08-28 20:37:41+05:30  Cprogrammer
 * Initial revision
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_QMAIL
#include <subfd.h>
#include <scan.h>
#include <open.h>
#include <strerr.h>
#include <env.h>
#include <str.h>
#include <sig.h>
#include <fmt.h>
#include <sgetopt.h>
#endif

#define FATAL             "server: fatal: "
#define SELECTTIMEOUT     5

char           *usage = "usage: server [-d timeout]";

void
out(char *str)
{
	if (!str || !*str)
		return;
	if (substdio_puts(subfdout, str) == -1)
		strerr_die2sys(111, FATAL, "write: ");
	return;
}

void
sigterm()
{
	substdio_flush(subfdout);
	substdio_flush(subfderr);
	_exit(1);
}

static char     strnum[FMT_ULONG];

int
main(int argc, char **argv)
{
	int             opt, length, dataTimeout = -1, retval, fd;
	struct timeval  timeout;
	struct timeval *tptr;
	time_t          last_timeout;
	char            buffer[256];
	char           *ptr;
	fd_set          rfds;	/*- File descriptor mask for select -*/

	sig_termcatch(sigterm);
	while ((opt = getopt(argc, argv, "d:")) != opteof) {
		switch (opt) {
		case 'd':
			scan_int(optarg, &dataTimeout);
			break;
		}
	}
	if ((fd = open_trunc("/tmp/dietpiserver.pid")) == -1)
		strerr_die2sys(111, FATAL, "/tmp/dietpiserver.pid: ");
	if (write(fd, strnum, fmt_ulong(strnum, getpid())) == -1)
		strerr_die2sys(111, FATAL, "write: /tmp/dietpiserver.pid: ");
	if (close(fd) == -1)
		strerr_die2sys(111, FATAL, "write: /tmp/dietpiserver.pid: ");
	if (dataTimeout == -1) {
		if (!(ptr = env_get("DATA_TIMEOUT")))
			ptr = "1800";
		scan_int(ptr, &dataTimeout);
	}
	if (dataTimeout > 0)
		timeout.tv_sec = (dataTimeout > SELECTTIMEOUT) ? dataTimeout : SELECTTIMEOUT;
	else
		timeout.tv_sec = 0 - dataTimeout;
	timeout.tv_usec = 0;
	tptr = (timeout.tv_sec ? &timeout : (struct timeval *) 0);

	last_timeout = timeout.tv_sec;
	for (;;) {
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		if ((retval = select(1, &rfds, (fd_set *) NULL, (fd_set *) NULL, tptr)) < 0) {
#ifdef ERESTART
			if (errno == EINTR || errno == ERESTART)
#else
			if (errno == EINTR)
#endif
				continue;
		}
		if (!retval) { /*- timeout occurred */
			if (dataTimeout > 0) {
				last_timeout += 2 * last_timeout;
				timeout.tv_sec = last_timeout;
			} else {
				timeout.tv_sec = 0 - dataTimeout;
				last_timeout = timeout.tv_sec;
			}
			timeout.tv_usec = 0;
		} else {
			if (dataTimeout > 0)
				last_timeout = timeout.tv_sec = (dataTimeout > SELECTTIMEOUT) ? dataTimeout : SELECTTIMEOUT;
			else
				timeout.tv_sec = 0 - dataTimeout;
			timeout.tv_usec = 0;
		}
		if (FD_ISSET(0, &rfds)) { /*- data from socket/stdin (client) */
			if ((length = read(0, buffer, sizeof(buffer))) == -1)
				strerr_die2sys(111, FATAL, "read-network: ");
			if (!length) {
				if (substdio_puts(subfderr, "shutting down\n") == -1)
					strerr_die2sys(111, FATAL, "write: ");
				if (substdio_flush(subfderr) == -1)
					strerr_die2sys(111, FATAL, "write: ");
				break;
			} 
#ifdef DEBUG
			strnum[fmt_ulong(strnum, length)] = 0;
			out("length = ");
			out(strnum);
			out("\n");
			substdio_flush(subfdout);
#endif
			if (!str_diffn(buffer, "shutdown", 8)) {
				execl("/sbin/shutdown", "-h", "now", (char *) 0);
				strerr_die2sys(111, FATAL, "execl: /sbin/shutdown -h now: ");
			} else
			if (!str_diffn(buffer, "quit", 4)) {
				close(0);
				close(1);
				close(2);
				_exit(0);
			} else
			if (!str_diffn(buffer, "wait", 4))
				continue;
			/*- display data from client */
			if (substdio_put(subfderr, buffer, length) == -1)
				strerr_die2sys(111, FATAL, "write: ");
			if (substdio_flush(subfderr) == -1)
				strerr_die2sys(111, FATAL, "write: ");
		}
	} /*- for (;;) { */
	_exit(0);
}

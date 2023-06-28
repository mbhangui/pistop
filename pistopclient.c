/*
 * $Log: pistopclient.c,v $
 * Revision 1.2  2023-06-28 12:34:04+05:30  Cprogrammer
 * added more info messages
 *
 * Revision 1.1  2022-07-16 16:35:32+05:30  Cprogrammer
 * Initial revision
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <subfd.h>
#include <scan.h>
#include <stralloc.h>
#include <wait.h>
#include <strerr.h>
#include <error.h>
#include <env.h>
#include <str.h>
#include <sig.h>
#include <fmt.h>
#include <sgetopt.h>

#define FATAL             "pistopclient: fatal: "
#define SELECTTIMEOUT     5

char           *usage = "usage: pistopclient [-d timeout]";

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
flush()
{
	if (substdio_flush(subfdout) == -1)
		strerr_die2sys(111, FATAL, "write: ");
}

void
err(char *str)
{
	if (!str || !*str)
		return;
	if (substdio_puts(subfderr, str) == -1)
		strerr_die2sys(111, FATAL, "write: ");
	return;
}

void
errf(char *str)
{
	if (!str || !*str)
		return;
	if (substdio_puts(subfderr, str) == -1)
		strerr_die2sys(111, FATAL, "write: ");
	else
	if (substdio_flush(subfderr) == -1)
		strerr_die2sys(111, FATAL, "write: ");
	return;
}

void
die_nomem()
{
	errf("out of memory\n");
	_exit(111);
}

void
die_read()
{
	errf("read error\n");
	_exit(111);
}

void
die_write()
{
	errf("write error\n");
	_exit(111);
}

void
die_alarm()
{
	errf("timeout\n");
	_exit(111);
}

void
sigterm()
{
	substdio_flush(subfdout);
	substdio_flush(subfderr);
	_exit(1);
}

int
timeoutread(t, fd, buf, len)
	int             t;
	int             fd;
	char           *buf;
	int             len;
{
	fd_set          rfds;
	struct timeval  tv;

	tv.tv_sec = t;
	tv.tv_usec = 0;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	if (select(fd + 1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv) == -1)
		return -1;
	if (FD_ISSET(fd, &rfds))
		return read(fd, buf, len);

	errno = error_timeout;
	return -1;
}

int
timeoutwrite(t, fd, buf, len)
	int             t;
	int             fd;
	char           *buf;
	int             len;
{
	fd_set          wfds;
	struct timeval  tv;

	tv.tv_sec = t;
	tv.tv_usec = 0;

	FD_ZERO(&wfds);
	FD_SET(fd, &wfds);

	if (select(fd + 1, (fd_set *) 0, &wfds, (fd_set *) 0, &tv) == -1)
		return -1;
	if (FD_ISSET(fd, &wfds))
		return write(fd, buf, len);

	errno = error_timeout;
	return -1;
}

void
do_start(char *pistopstart)
{
	pid_t           child;
	int             wstat;

	out("executing ");
	out(pistopstart);
	out(" start\n");
	flush();
	switch((child = fork()))
	{
	case -1:
		strerr_die2sys(111, FATAL, "fork: ");
	case 0:
		execl(pistopstart, "pistopstart", "start", (char *) 0);
		strerr_die4sys(111, FATAL, "execl: ", pistopstart, " start: ");
	default:
		break;
	}
	if (wait_pid(&wstat, child) == -1)
		strerr_die2sys(111, FATAL, "wait: ");
	if (wait_crashed(wstat))
		strerr_die2sys(111, FATAL, "wait: ");
	switch (wait_exitcode(wstat))
	{
	case 0:
		return;
	default:
		strerr_die2x(0, FATAL, "client startup failed");
	}
}

int
main(int argc, char **argv)
{
	int             opt, length, dataTimeout = -1, r;
	struct timeval  timeout;
	struct timeval *tptr;
	time_t          last_timeout;
	char            buffer[256];
	stralloc        pistopstart = {0};
	char           *ptr;
	fd_set          rfds; /*- File descriptor mask for select -*/

	sig_termcatch(sigterm);
	while ((opt = getopt(argc, argv, "d:")) != opteof) {
		switch (opt) {
		case 'd':
			scan_int(optarg, &dataTimeout);
			break;
		}
	}
	if (!(ptr = env_get("HOME"))) {
		errf("HOME not set\n");
		_exit(100);
	}
	if (chdir(ptr) == -1 || chdir(".pistop")== -1 )
		strerr_die4sys(111, FATAL, "chdir: ", ptr, "/.pistopstart: ");
	if (!access("pistopstart", X_OK)) {
		if (!stralloc_copys(&pistopstart, ptr) ||
				!stralloc_catb(&pistopstart, "/.pistop/pistopstart", 20) ||
				!stralloc_0(&pistopstart))
			die_nomem();
	} else
	if (!access(LIBEXECDIR"/pistop/pistopstart", X_OK)) {
		if (!stralloc_copys(&pistopstart, LIBEXECDIR) ||
				!stralloc_catb(&pistopstart, "/pistop/pistopstart", 19) ||
				!stralloc_0(&pistopstart))
			die_nomem();
	} else {
		err("pistopstart not found in ");
		err(LIBEXECDIR"/pistop");
		err(" and ");
		err(ptr);
		errf("/.pistop\n");
		_exit(100);
	}
	do_start(pistopstart.s);
	if (dataTimeout == -1) {
		if (!(ptr = env_get("DATA_TIMEOUT")))
			ptr = "120";
		scan_int(ptr, &dataTimeout);
	}
	if ((r = timeoutwrite(dataTimeout, 7, "wait\n", 5)) < 0)
		die_write();
	if (dataTimeout > 0)
		timeout.tv_sec = (dataTimeout > SELECTTIMEOUT) ? dataTimeout : SELECTTIMEOUT;
	else
		timeout.tv_sec = 0 - dataTimeout;
	timeout.tv_usec = 0;
	tptr = (timeout.tv_sec ? &timeout : (struct timeval *) 0);

	last_timeout = timeout.tv_sec;
	for (;;) {
		FD_ZERO(&rfds);
		FD_SET(6, &rfds);
		if ((r = select(7, &rfds, (fd_set *) NULL, (fd_set *) NULL, tptr)) < 0) {
#ifdef ERESTART
			if (errno == EINTR || errno == ERESTART)
#else
			if (errno == EINTR)
#endif
				continue;
		}
		if (!r) { /*- timeout occurred */
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
		if (FD_ISSET(6, &rfds)) { /*- data from socket/stdin and display it */
			if ((length = read(6, buffer, sizeof(buffer))) == -1)
				strerr_die2sys(111, FATAL, "read-network: ");
			if (!length) { /*- server died or network disconnect */
				close(6);
				out("lost connection with remote. executing ");
				out(pistopstart.s);
				out(" stop\n");
				flush();
				execl(pistopstart.s, "pistopstart", "stop", (char *) 0);
				strerr_die4sys(111, FATAL, "execl: ", pistopstart.s, " stop: ");
				_exit(0);
			} 
			if (substdio_put(subfderr, "got command [", 13) == -1 ||
					substdio_put(subfderr, buffer, length) == -1 ||
					substdio_put(subfderr, "]\n", 2) == -1 || substdio_flush(subfderr))
				die_write();
			if (!str_diffn(buffer, "shutdown\n", 9)) {
				out("remote powered off executing ");
				out(pistopstart.s);
				out(" stop\n");
				flush();
				execl(pistopstart.s, "pistopstart", "stop", (char *) 0);
				strerr_die4sys(111, FATAL, "execl: ", pistopstart.s, " stop: ");
				close(6);
				_exit(0);
			}
		}
	} /*- for (;;) { */
	_exit(0);
}

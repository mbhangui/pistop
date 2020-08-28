/*
 * $Log: $
 */
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <subfd.h>
#include <scan.h>
#include <stralloc.h>
#include <strerr.h>
#include <error.h>
#include <env.h>
#include <str.h>
#include <sig.h>
#include <fmt.h>
#include <sgetopt.h>

#define FATAL             "client: fatal: "
#define SELECTTIMEOUT     5

char           *usage = "usage: client [-d timeout]";

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

int
main(int argc, char **argv)
{
	int             opt, length, dataTimeout = -1, r;
	struct timeval  timeout;
	struct timeval *tptr;
	time_t          last_timeout;
	char            buffer[256];
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
	if (dataTimeout == -1) {
		if (!(ptr = env_get("DATA_TIMEOUT")))
			ptr = "1800";
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
			if (!length) {
				out("PI4 powerd off\n");
				flush();
				/*
				execl("/sbin/shutdown", "-h", "now", (char *) 0);
				strerr_die2sys(111, FATAL, "execl: /sbin/shutdown -r now: ");
				*/
			} 
			if (substdio_put(subfderr, buffer, length) == -1)
				die_write();
			if (substdio_flush(subfderr) == -1)
				die_write();
		}
	} /*- for (;;) { */
	_exit(0);
}

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([pistop],m4_normalize(m4_include([conf-version])),m4_normalize(m4_include([conf-email])),pistop,[https://github.com/mbhangui/pistop])
AC_CONFIG_SRCDIR([pistopserver.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# Checks for libraries.
AC_CHECK_LIB(qmail, substdio_fdbuf, [AC_SUBST([LIB_QMAIL], ["-lqmail"]) AC_DEFINE([HAVE_QMAIL], [1],[qmail Library])],noqmail=t,)
if test " $noqmail" = " t"
then
  AC_MSG_ERROR(Could not find qmail library.)
fi

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h errno.h sys/select.h sys/types.h sys/mount.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([select])

servicedir="/service"
AC_ARG_ENABLE(servicedir, [  --enable-servicedir=dir         directory where supervise services are installed.],
	servicedir="$enableval",
	[
		if test "$servicedir" = ""
		then
			AC_MSG_ERROR([Unable to find your servicedir directory, specify --enable-servicedir.])
		fi
	])
AC_DEFINE_UNQUOTED(SERVICEDIR,"$servicedir",supervise service directory)
AC_DEFINE_UNQUOTED(LIBEXECDIR,"$libexecdir", libexec directory)
AC_SUBST(servicedir)
AC_SUBST(prefix)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([pistop-release:pistop-release.in],[], [])
AC_OUTPUT

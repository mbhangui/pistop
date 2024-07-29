#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([pistop],m4_normalize(m4_include([conf-version])),m4_normalize(m4_include([conf-email])),pistop,[https://github.com/mbhangui/pistop])
AC_CANONICAL_HOST
AC_CONFIG_SRCDIR([pistopserver.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC

case "$host" in
*-*-sunos4.1.1*)
	CPPFLAGS="$CPPFLAGS -DSUNOS4"
	CFLAGS="$CFLAGS -O4 -Wall -fPIC"
	CXXFLAGS="$CXXFLAGS -O4 -Wall -fPIC"
	;;
*-*-solaris*)
	CPPFLAGS="$CPPFLAGS -DSOLARIS"
	CFLAGS="$CFLAGS -O4 -Wall -fPIC"
	CXXFLAGS="$CXXFLAGS -O4 -Wall -fPIC"
	;;
*-*-linux*)
	CPPFLAGS="$CPPFLAGS -DLINUX"
	CFLAGS="$CFLAGS -O4 -Wall -fPIC"
	CXXFLAGS="$CXXFLAGS -DLINUX -O4 -Wno-delete-non-virtual-dtor -Wno-reorder -Wall -fPIC"
	LDFLAGS="$LDFLAGS -pie"
	;;
*-*-freebsd*)
	CPPFLAGS="$CPPFLAGS -DFREEBSD -I/usr/local/include"
	CFLAGS="$CFLAGS -I/usr/local/include -I/usr/local/include/qmail -Wall"
	CXXFLAGS="$CXXFLAGS -DFREEBSD -I/usr/local/include -Wall -fPIC"
	CXXFLAGS="$CXXFLAGS -Wno-delete-non-virtual-dtor -Wno-deprecated-register -Wno-reorder"
	CXXFLAGS="$CXXFLAGS -Wno-delete-non-abstract-non-virtual-dtor"
	LDFLAGS="$LDFLAGS -L/usr/local/lib"
	;;
*-*-darwin*)
	CPPFLAGS="$CPPFLAGS -DDARWIN -I/opt/local/include"
	CFLAGS="$CFLAGS -I/opt/local/include -I/usr/local/include -I/opt/local/include/qmail -Wall"
	CXXFLAGS="$CXXFLAGS -DDARWIN -Wall -fPIC"
	CXXFLAGS="$CXXFLAGS -DBIND_8_COMPAT -I/opt/local/include -Wno-c++11-extensions"
	CXXFLAGS="$CXXFLAGS -Wno-delete-non-virtual-dtor -Wno-reorder-ctor -Wno-reorder"
	LDFLAGS="$LDFLAGS -L/opt/local/lib -L/usr/local/lib"
	;;
*)
	CFLAGS="$CFLAGS -O4 -Wall -fPIC"
	CXXFLAGS="$CXXFLAGS -O4 -Wall -fPIC"
	;;
esac

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

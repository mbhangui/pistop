@SET_MAKE@
COFLAGS=-z+05:30
ACLOCAL_AMFLAGS=-I m4

INSTALL_PROGRAM = ${INSTALL} -c
install_sh_PROGRAM = $(install_sh) -c
INSTALL_SCRIPT = ${INSTALL}
install_sh_SCRIPT = $(install_sh) -c

DEFS=@DEFS@ -fno-strict-aliasing -I/usr/include/qmail

bindir=@prefix@/bin

pistopsysconfdir=@sysconfdir@/pistop
libexec_PROGRAMS = server$(EXEEXT)
libexec_SCRIPTS = create_service client backup
noinst_DATA = pistop.spec

server_SOURCES = server.c
server_LDADD = $(LIB_QMAIL)

MOSTLYCLEANFILES = create_services client

edit = sed \
	-e 's}@PACKAGE\@}$(PACKAGE)}g' \
	-e 's}@PACKAGE_VERSION\@}$(PACKAGE_VERSION)}g' \
	-e 's}@version\@}$(VERSION)}g' \
	-e "s|@release\@|"`cat conf-release`"|g" \
	-e 's}@pkgconfigdir\@}$(pkgconfigdir)}g' \
	-e 's}@VERSION\@}$(VERSION)}g' \
	-e 's}@pistopsysconfdir\@}$(pistopsysconfdir)}g' \
	-e 's}@prefix\@}$(prefix)}g'

create_service: create_service.in
	$(edit) $@.in > $@
	chmod +x $@
client: client.in
	$(edit) $@.in > $@
	chmod +x $@
backup: backup.in
	$(edit) $@.in > $@
	chmod +x $@
pistop.spec: pistop.spec.in catChangeLog ChangeLog conf-version conf-release
	(cat $@.in;./catChangeLog) | $(edit) > $@

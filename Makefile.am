@SET_MAKE@
COFLAGS=-z+05:30
ACLOCAL_AMFLAGS=-I m4

INSTALL_PROGRAM = ${INSTALL} -c
install_sh_PROGRAM = $(install_sh) -c
INSTALL_SCRIPT = ${INSTALL}
install_sh_SCRIPT = $(install_sh) -c

DEFS=@DEFS@ -fno-strict-aliasing -I/usr/include/qmail

bindir=@prefix@/bin

libexec_PROGRAMS = server$(EXEEXT)
libexec_SCRIPTS = create_service client backup
noinst_DATA = pistop.spec PKGBUILD
SYSCONFDIR=@sysconfdir@
sysconf_DATA=pistop-release
doc_DATA=README.md COPYING pistop.changes

server_SOURCES = server.c
server_LDADD = $(LIB_QMAIL)

MOSTLYCLEANFILES = create_services client

edit = sed \
	-e 's}@PACKAGE\@}$(PACKAGE)}g' \
	-e 's}@PACKAGE_VERSION\@}$(PACKAGE_VERSION)}g' \
	-e 's}@version\@}$(VERSION)}g' \
	-e "s|@release\@|"`cat conf-release`"|g" \
	-e 's|@email\@|'"`cat conf-email`"'|g' \
	-e 's}@pkgconfigdir\@}$(pkgconfigdir)}g' \
	-e 's}@VERSION\@}$(VERSION)}g' \
	-e 's}@sysconfdir\@}$(sysconfdir)}g' \
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
pistop.spec: pistop.spec.in catChangeLog doc/ChangeLog conf-version \
conf-release conf-email
	(cat $@.in;./catChangeLog) | $(edit) > $@
pistop.changes: doc/ChangeLog conf-version conf-release conf-email
	./catChangeLog --changes doc/ChangeLog > $@
PKGBUILD: PKGBUILD.in conf-email \
conf-version conf-release pistop.changes
	cat $@.in | $(edit) > $@

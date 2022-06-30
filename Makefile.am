@SET_MAKE@
COFLAGS=-z+05:30
ACLOCAL_AMFLAGS=-I m4

INSTALL_PROGRAM = ${INSTALL} -c
install_sh_PROGRAM = $(install_sh) -c
INSTALL_SCRIPT = ${INSTALL}
install_sh_SCRIPT = $(install_sh) -c

DEFS=@DEFS@ -fno-strict-aliasing -I/usr/include/qmail

pistopbindir=@prefix@/bin
pistoplibexecdir=@libexecdir@/pistop
pistopsysconfdir=@sysconfdir@/pistop

pistoplibexec_PROGRAMS = pistopclient$(EXEEXT) pistopserver$(EXEEXT)
pistoplibexec_SCRIPTS = create_service backup pistopstart
noinst_DATA = pistop.spec PKGBUILD
pistopsysconf_DATA=pistop-release
doc_DATA=README.md COPYING pistop.changes
man_MANS=pistopclient.1 pistopserver.1

pistopserver_SOURCES = pistopserver.c
pistopserver_LDADD = $(LIB_QMAIL)

pistopclient_SOURCES = pistopclient.c
pistopclient_LDADD = $(LIB_QMAIL)

MOSTLYCLEANFILES = create_services pistopstart pistopserver.1

edit = sed \
	-e 's}@PACKAGE\@}$(PACKAGE)}g' \
	-e 's}@PACKAGE_VERSION\@}$(PACKAGE_VERSION)}g' \
	-e 's}@version\@}$(VERSION)}g' \
	-e "s|@release\@|"`cat conf-release`"|g" \
	-e 's|@email\@|'"`cat conf-email`"'|g' \
	-e 's}@pkgconfigdir\@}$(pkgconfigdir)}g' \
	-e 's}@VERSION\@}$(VERSION)}g' \
	-e 's}@servicedir\@}$(servicedir)}g' \
	-e 's}@sysconfdir\@}$(sysconfdir)}g' \
	-e 's}@pistoplibexecdir\@}$(pistoplibexecdir)}g' \
	-e 's}@prefix\@}$(prefix)}g'

create_service: create_service.in
	$(edit) $@.in > $@
	chmod +x $@
client: client.in
	$(edit) $@.in > $@
	chmod +x $@
pistopstart: pistopstart.in
	$(edit) $@.in > $@
	chmod +x $@
backup: backup.in
	$(edit) $@.in > $@
	chmod +x $@
pistopserver.1: pistopserver.9
	$(edit) pistopserver.9 > $@
pistop.spec: pistop.spec.in catChangeLog doc/ChangeLog conf-version \
conf-release conf-email
	(cat $@.in;./catChangeLog) | $(edit) > $@
pistop.changes: doc/ChangeLog conf-version conf-release conf-email
	./catChangeLog --changes doc/ChangeLog > $@
PKGBUILD: PKGBUILD.in conf-email \
conf-version conf-release pistop.changes
	cat $@.in | $(edit) > $@

prefix=/usr
libexecdir=/usr/libexec
mandir=/usr/share/man
sysconfdir=/etc
logdir=/var/log/svc
servicedir=/service
version=$(shell cat ../conf-version)
release=$(shell cat ../conf-release)
email=$(shell cat ../conf-email)

edit = sed \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@sysconfdir\@,$(sysconfdir),g' \
	-e 's,@shareddir\@,$(shareddir),g' \
	-e 's,@libexecdir\@,$(libexecdir),g' \
	-e 's,@mandir\@,$(mandir),g' \
	-e 's,@logdir\@,$(logdir),g' \
	-e 's,@servicedir\@,$(servicedir),g' \
	-e 's,@version\@,$(version),g' \
	-e 's,@release\@,$(release),g' \
	-e 's,@email\@,$(email),g' \
	-e 's,@DATE\@,$(DATE),g'

all: rules changelog preinst postinst prerm \
	pistop.dsc pistop.install \
	debian.tar.gz

rules: rules.in Makefile ../conf-version ../conf-release ../conf-email
	$(edit) $@.in > $@;chmod +x rules

pistop.install: pistop.install.in
	$(edit) $@.in > $@

changelog: ../doc/ChangeLog Makefile ../conf-version ../conf-release \
../conf-email
	../catChangeLog --debian --name=pistop --state=unstable \
		--urgency=low ../doc/ChangeLog > $@
preinst: preinst.in
	$(edit) $@.in > $@
postinst: postinst.in
	$(edit) $@.in > $@
prerm: prerm.in
	$(edit) $@.in > $@
control: control.in ../conf-email
	$(edit) $@.in > $@
copyright: copyright.in ../conf-email
	$(edit) $@.in > $@

pistop.dsc: pistop.dsc.in Makefile ../conf-version ../conf-release \
../conf-email
	$(edit) $@.in > $@

debian.tar.gz: copyright rules changelog compat docs \
Makefile control pistop.install preinst postinst prerm
	cd .. && tar cf - debian/copyright debian/rules debian/changelog \
	debian/docs debian/source debian/compat debian/control \
	debian/pistop.install debian/preinst debian/postinst debian/prerm \
	| gzip > debian/$@

clean:
	/bin/rm -f rules *.dsc changelog \
	pistop.install prerm preinst postinst debian.tar.gz

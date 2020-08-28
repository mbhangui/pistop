prefix=/usr
libexecdir=/usr/libexec/pistop
sysconfdir=/etc/pistop
logdir=/var/log/svc
servicedir=/service
version=$(shell cat ../conf-version)
release=$(shell cat ../conf-release)
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
	-e 's,@DATE\@,$(DATE),g'

all: rules changelog \
	pistop.dsc pistop.install \
	debian.tar.gz

rules: rules.in Makefile ../conf-version ../conf-release
	$(edit) $@.in > $@;chmod +x rules

pistop.install: pistop.install.in
	$(edit) $@.in > $@

changelog: changelog.in ../conf-version ../conf-release
	(cat $@.in; echo ""; \
	echo " -- Manvendra Bhangui <pistop@indimail.org>  `date +'%a, %d %b %Y %H:%M:%S %z'`" ) \
	| $(edit) > $@

pistop.dsc: pistop.dsc.in Makefile ../conf-version ../conf-release
	$(edit) $@.in > $@

debian.tar.gz: copyright rules changelog compat \
Makefile control pistop.install
	tar cf - copyright rules changelog \
	compat control pistop.install | gzip > $@

clean:
	/bin/rm -f rules *.dsc changelog \
	pistop.install debian.tar.gz

PROGNM  =  pandabin
PREFIX  ?= /srv/http
MAINDIR ?= $(DESTDIR)$(PREFIX)
SVCDIR  ?= $(DESTDIR)/usr/lib/systemd/system/
BINDIR  ?= $(DESTDIR)/usr/bin
TARGET  ?= oceanus.halosgho.st
PORT    ?= 2222

include Makerules

.PHONY: all bin clean complexity scan-build cov-build res minify install uninstall

all: dist bin

bin: dist
	@(cd src; \
		$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o ../dist/$(PROGNM); \
		$(CC) $(CFLAGS) $(LDFLAGS) redirector.c -o ../dist/redirector \
	)

scan-build:
	@scan-build --use-cc=$(CC) make bin

clean:
	@rm -rf -- dist cov-int $(PROGNM).tgz ./src/*.plist
	@rm -rf -- bld/{lwan-git,acme-client-git,hitch-git,pkg,src,packages,halosgho.st}

complexity:
	@complexity -h ./src/*

cov-build: clean dist
	@cov-build --dir cov-int make
	@tar czvf $(PROGNM).tgz cov-int

dist:
	@mkdir -p dist/.well-known/acme-challenge

res: dist
	@cp -a --no-preserve=ownership pages dist/
	@cp -a --no-preserve=ownership media dist/
	@cp -a --no-preserve=ownership assets dist/

minify: res
	@(cd dist; \
	for i in assets/*.css pages/*.html; do \
		mv "$$i" "$$i".bak; \
		sed -E 's/^\s+//g' "$$i".bak | tr -d '\n' > "$$i"; \
		rm "$$i".bak; \
	done)

install: all
	@mkdir -p $(BINDIR) $(SVCDIR) $(MAINDIR)
	@cp -a --no-preserve=ownership dist/* $(MAINDIR)/
	@cp -a --no-preserve=ownership svc/* $(SVCDIR)/
	@install -m755 -t $(BINDIR) bin/*

deploy:
	@(pushd bld; \
	mkdir -p packages; \
	for i in lwan-git hitch-git acme-client-git; do \
		cower -df "$$i" --ignorerepo &> /dev/null; \
		pushd "$$i"; \
		PKGDEST=../packages makepkg -s; \
		popd; \
		echo "$$i: built"; \
	done; \
	PKGDEST=packages makepkg -s; \
	scp -P $(PORT) -r packages $(TARGET):/home/halosghost/; \
	ssh -p $(PORT) $(TARGET); \
	)

uninstall:
	@rm -rf -- $(MAINDIR)/{assets,media,pages,.well-known}
	@rm -f  -- $(SVCDIR)/{$(PROGNM),hgredirector}.service
	@rm -f  -- $(BINDIR)/website

include Makeeaster

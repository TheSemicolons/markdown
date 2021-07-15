#
# Copyright (c) 2018 Jim Anderson <thesemicolons@protonmail.com>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all
# copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
# WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
# AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
# DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
# PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
# TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
#
#CFLAGS  = -fpic -I/usr/local/include
#LDFLAGS = -L/usr/local/lib

.POSIX:
.SUFFIXES:

CC        = clang
CFLAGS    += -Wall -Werror -Wextra
CFLAGS    += -Wcast-align -Wconversion -Wfloat-equal
CFLAGS    += -Wformat=2 -Wimplicit-atomic-properties -Wcast-qual -Wsign-compare
CFLAGS    += -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes
CFLAGS    += -Woverlength-strings -Wshadow -Wstrict-selector-match
CFLAGS    += -Wundeclared-selector -Wunreachable-code -Wpointer-arith
CFLAGS    += -fPIC -pedantic -emit-llvm -O3
LDFLAGS   = -fuse-ld=lld
PREFIX    = /usr/local
MANPREFIX = $(PREFIX)/man
VERSION   = 0.1

all: tergano

install: tergano
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f tergano $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/tergano
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp tergano.1 $(DESTDIR)$(MANPREFIX)/man1/tergano.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/tergano.1

dist:
	rm -rf tergano-$(VERSION)
	mkdir -p tergano-$(VERSION)
	cp -R CHANGELOG CONTRIBUTING LICENSE README *.h *.c tergano-$(VERSION)/
	tar -czf tergano-$(VERSION) tergano-$(VERSION).tgz
	rm -Rf tergano-$(VERSION)

tergano: listing.bc markdown.bc readfile.bc tergano.bc
	$(CC) $(LDFLAGS) -o tergano listing.bc markdown.bc readfile.bc tergano.bc

listing.bc: listing.c listing.h
	$(CC) -c $(CFLAGS) listing.c

markdown.bc: markdown.c markdown.h
	$(CC) -c $(CFLAGS) markdown.c

readfile.bc: readfile.c readfile.h
	$(CC) -c $(CFLAGS) readfile.c

tergano.bc: tergano.c
	$(CC) -c $(CFLAGS) tergano.c

clean:
	rm -f *.bc *.core

distclean: clean
	rm -f tergano
	rm -f *.tgz


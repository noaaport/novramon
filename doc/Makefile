#
# $Id$
#
.include "../Makefile.inc"

INSTALL = install

DOCFILES = novramon.README LICENSE
SRC =           novramon.man
TARGET =        novramon.${MANEXT}.gz

all: ${TARGET}

${TARGET}: ${SRC}
	 gzip -c ${SRC} > ${TARGET}

install: all
	${INSTALL} -m 644 ${TARGET} ${MYMANDIR}
	${INSTALL} -m 0644 ${DOCFILES}  ${MYDOCDIR}

clean:
	rm -f *~ ${TARGET}

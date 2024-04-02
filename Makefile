# light-light-screenshot - screenshot utility for X11
# See LICENSE file for copyright and license details.

include config.mk

MODE ?= release
OUT_DIR = out/${MODE}
DIST_DIR = dist

SRC = helper.c
HEADERS = utils.h
OBJ = $(addprefix ${OUT_DIR}/,${SRC:.c=.o})
DIST_ASSETS = light-screenshot LICENSE Makefile README.md config.mk ${HEADERS} ${SRC}

ifeq (${MODE}, release)
	CFLAGS += ${RELEASE_CFLAGS}
endif

all: options build

options:
	@echo ${BIN_NAME} build options \(${MODE}\):
	@echo ""
	@echo "CFLAGS   = ${CFLAGS} ${DEFFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"
	@echo ""

${OUT_DIR}:
	mkdir -p $@

${OUT_DIR}/%.o: %.c ${HEADERS} config.mk | ${OUT_DIR}
	${CC} -c ${CFLAGS} ${DEFFLAGS} $< -o $@

${OUT_DIR}/${BIN_NAME}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

build: | ${OUT_DIR}/${BIN_NAME}

clean:
	rm -rf ${OUT_DIR}
	rm -rf ${DIST_DIR}

dist:
	mkdir -p ${DIST_DIR}/${BIN_NAME}-${VERSION}
	cp -r ${DIST_ASSETS} ${DIST_DIR}/${BIN_NAME}-${VERSION}
	cd ${DIST_DIR}; \
		tar -cf ${BIN_NAME}-${VERSION}.tar ${BIN_NAME}-${VERSION}; \
		gzip ${BIN_NAME}-${VERSION}.tar; \
		rm -rf ${BIN_NAME}-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${OUT_DIR}/${BIN_NAME} ${DESTDIR}${PREFIX}/bin
	cp -f light-screenshot ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${BIN_NAME}

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${BIN_NAME}
	rm -f ${DESTDIR}${PREFIX}/bin/light-screenshot

.PHONY: all options clean build dist install uninstall

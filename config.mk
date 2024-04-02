# app version
VERSION = 0.1.0
BIN_NAME = light-screenshot-helper

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# flags
DEFFLAGS = -D_DEFAULT_SOURCE -DVERSION=\"${VERSION}\"

RELEASE_CFLAGS = \
	-ffast-math \
    -fno-finite-math-only \
    -march=native \
    -fno-exceptions \
	-Ofast \
	-fforce-emit-vtables \
	-faddrsig

CFLAGS = \
	-std=c17 \
	-pedantic \
	-Wall \
	-Wno-deprecated-declarations \
	-I/usr/include \

LDFLAGS = \
	-lX11 -lpng

# compiler and linker
CC = clang

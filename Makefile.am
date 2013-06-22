AM_CFLAGS=-Wall -Wextra -pedantic -std=c11 -I./src -I./include

lib_LTLIBRARIES=libdogma.la
libdogma_ladir=.

libdogma_la_SOURCES=src/dogma.c include/dogma.h src/dogma_internal.c	\
	src/dogma_internal.h src/tables.c src/tables.h src/eval.c			\
	src/eval.h src/modifier.c src/modifier.h src/attribute.c			\
	src/attribute.h src/libdogma.sym src/operands.h src/tables-data.h	\
	test/test.h tools/bootstrap_from_json

libdogma_la_LDFLAGS=-export-symbols src/libdogma.sym -version-info 0:0:0
include_HEADERS=include/dogma.h

check_PROGRAMS=test/trivial test/basic-attributes				\
	test/basic-skill-levels test/basic-charges					\
	test/stacking-penalties test/ancillary-modules test/drones	\
	test/implants test/projected-effects test/bruteforce

LDADD=libdogma.la
TESTS=$(check_PROGRAMS)

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA = libdogma.pc

AM_CFLAGS=-Wall -Wextra -pedantic -std=c11 -I./src -I./src/core	\
-I./src/extra -I./include

lib_LTLIBRARIES=libdogma.la
libdogma_ladir=.

libdogma_la_SOURCES=src/core/dogma.c include/dogma.h			\
	src/core/dogma_internal.c src/core/dogma_internal.h			\
	src/core/tables.c src/core/tables.h src/core/eval.c			\
	src/core/eval.h src/core/modifier.c src/core/modifier.h		\
	src/core/attribute.c src/core/attribute.h src/core/fleet.c	\
	src/core/fleet.h src/extra/extra.c src/libdogma.sym			\
	src/operands.h src/tables-data.h test/test.h				\
	tools/bootstrap_from_json

libdogma_la_LDFLAGS=-export-symbols src/libdogma.sym -version-info 0:0:0
include_HEADERS=include/dogma.h include/dogma-names.h	\
	include/dogma-extra.h

check_PROGRAMS=test/trivial test/basic-attributes				\
	test/basic-skill-levels test/basic-charges test/basic-fleet	\
	test/stacking-penalties test/ancillary-modules test/drones	\
	test/implants test/projected-effects test/fleet-stacking	\
	test/self-on-self test/extra-misc test/bruteforce

LDADD=libdogma.la
TESTS=$(check_PROGRAMS)

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA = libdogma.pc

AM_CFLAGS=-std=c11 -I./src

lib_LTLIBRARIES=libdogma.la
libdogma_ladir=.
dist_libdogma_la_SCRIPTS=tools/bootstrap_from_json
dist_libdogma_la_DATA=src/libdogma.sym
libdogma_la_SOURCES=src/dogma.c src/dogma_internal.h src/tables.c src/tables.h
libdogma_la_LDFLAGS=-export-symbols src/libdogma.sym -version-info 0:0:0
include_HEADERS=src/dogma.h

check_PROGRAMS=test/trivial
LDADD=libdogma.la
TESTS=$(check_PROGRAMS)

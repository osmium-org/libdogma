#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libdogma], [0.1.0])
AM_INIT_AUTOMAKE([no-dist-gzip dist-xz no-define foreign silent-rules subdir-objects color-tests])
AM_SILENT_RULES([yes])
LT_INIT
AC_CONFIG_SRCDIR([src/core/dogma.c])

AC_MSG_CHECKING([whether debug flags should be enabled])
AC_ARG_ENABLE(debug, AS_HELP_STRING([--enable-debug], [enable debug flags (default is no)]))
AS_IF([test "x$enable_debug" != "xyes"], [enable_debug=no])
AC_MSG_RESULT($enable_debug)

AS_IF([test "x$enable_debug" = "xyes"], [CFLAGS="-g -O0"])

AC_PROG_CC([clang gcc cc])

AC_SEARCH_LIBS([JudyLGet], [Judy], [], [AC_MSG_ERROR([unable to find Judy])])
AC_SEARCH_LIBS([sqrt], [m], [], [AC_MSG_ERROR([unable to link math library])])
AC_CHECK_HEADERS([stdlib.h stdint.h stdbool.h assert.h math.h Judy.h], [], [AC_MSG_ERROR([unable to find some headers])])

AC_CONFIG_FILES([Makefile libdogma.pc])
AC_OUTPUT

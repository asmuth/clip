## Process this file with autoconf to produce configure.
## In general, the safest way to proceed is to run ./autogen.sh

AC_PREREQ(2.59)
AC_INIT([libtransport],[0.1.0-dev],[authors@fnordmetric.io],[libtransport])
AM_INIT_AUTOMAKE([foreign subdir-objects serial-tests])
AM_MAINTAINER_MODE([enable])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AM_SILENT_RULES([yes])
AC_LANG([C++])

# Check for build env, standard header files and library functions
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AS
AM_PROG_AR
AC_HEADER_STDC
AC_FUNC_MEMCMP
AC_FUNC_STRTOD
ACX_PTHREAD
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h stdlib.h unistd.h syslog.h])
AC_CHECK_FUNCS([ftruncate posix_fallocate posix_fadvise gethostbyname_r])
AM_CONDITIONAL([HAVE_SYSLOG_H], [test x$HAVE_SYSLOG_H = x1])
AM_CONDITIONAL([HAVE_GETHOSTBYNAME_R], [test x$HAVE_GETHOSTBYNAME_R = x1])
AM_CONDITIONAL([HAVE_POSIX_FALLOCATE], [test x$HAVE_POSIX_FALLOCATE = x1])
AM_CONDITIONAL([HAVE_POSIX_FADVISE], [test x$HAVE_POSIX_FADVISE = x1])
AM_CONDITIONAL([HAVE_PTHREAD], [test "x$acx_pthread_ok" = "xyes"])

# Check for zlib
HAVE_ZLIB=0
AS_IF([test "$with_zlib" != no], [
  AC_MSG_CHECKING([zlib version])

  # First check the zlib header version
  AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM([[
        #include <zlib.h>
        #if !defined(ZLIB_VERNUM) || (ZLIB_VERNUM < 0x1204)
        # error zlib version too old
        #endif
        ]], [])], [
    AC_MSG_RESULT([ok (1.2.0.4 or later)])

    # Also need to add -lz to the linker flags and make sure this succeeds.
    AC_SEARCH_LIBS([zlibVersion], [z], [
      AC_DEFINE([HAVE_ZLIB], [1], [Enable classes using zlib compression.])
      HAVE_ZLIB=1
    ], [
      AS_IF([test "$with_zlib" != check], [
        AC_MSG_FAILURE([--with-zlib was given, but no working zlib library was found])
      ])
    ])
  ], [
    AS_IF([test "$with_zlib" = check], [
      AC_MSG_RESULT([headers missing or too old (requires 1.2.0.4)])
    ], [
      AC_MSG_FAILURE([--with-zlib was given, but zlib headers were not present or were too old (requires 1.2.0.4)])
    ])
  ])
])
AM_CONDITIONAL([HAVE_ZLIB], [test $HAVE_ZLIB = 1])

AC_CONFIG_FILES([
    Makefile
    src/libtransport/http/Makefile
    src/libtransport/uri/Makefile
    test/Makefile
    examples/http_client/Makefile
    examples/http_server/Makefile
    examples/http_server_multithreaded/Makefile
    examples/http_server_streaming/Makefile
    examples/http_server_sse/Makefile])

AC_OUTPUT

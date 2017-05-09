## Process this file with autoconf to produce configure.
## In general, the safest way to proceed is to run ./autogen.sh

AC_PREREQ(2.59)
AC_INIT([metrictools],[0.8.0-dev],[authors@metrictools.org],[metrictools])
AM_INIT_AUTOMAKE([foreign subdir-objects serial-tests])
AM_MAINTAINER_MODE([enable])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([build])
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
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h stdlib.h unistd.h syslog.h])
AC_CHECK_FUNCS([ftruncate posix_fallocate posix_fadvise gethostbyname_r])
AM_CONDITIONAL([HAVE_SYSLOG_H], [test x$HAVE_SYSLOG_H = x1])
AM_CONDITIONAL([HAVE_GETHOSTBYNAME_R], [test x$HAVE_GETHOSTBYNAME_R = x1])
AM_CONDITIONAL([HAVE_POSIX_FALLOCATE], [test x$HAVE_POSIX_FALLOCATE = x1])
AM_CONDITIONAL([HAVE_POSIX_FADVISE], [test x$HAVE_POSIX_FADVISE = x1])
ACX_PTHREAD
ACX_ZLIB

# --enable-mysql
AC_MSG_CHECKING([Feature: MySQL Backend])
AC_ARG_ENABLE([mysql], AS_HELP_STRING([--enable-mysql], [Enable mysql]))
AM_CONDITIONAL([ENABLE_MYSQL], [test "x$enable_mysql" = "xyes"])
AC_SUBST(ENABLE_MYSQL)
if test "x$enable_mysql" = "xyes"; then
  AC_DEFINE([ENABLE_MYSQL], [1])
  AC_MSG_RESULT(["enabled"])
else
  AC_MSG_RESULT(["disabled"])
fi

# --enable-sqlite
AC_MSG_CHECKING([Feature: SQLite Backend])
AC_ARG_ENABLE([sqlite], AS_HELP_STRING([--enable-sqlite], [Enable sqlite]))
AM_CONDITIONAL([ENABLE_SQLITE], [test "x$enable_sqlite" = "xyes"])
AC_SUBST(ENABLE_SQLITE)
if test "x$enable_sqlite" = "xyes"; then
  AC_DEFINE([ENABLE_SQLITE], [1])
  AC_MSG_RESULT(["enabled"])
else
  AC_MSG_RESULT(["disabled"])
fi

AC_CONFIG_SUBDIRS([build/deps/libtransport])
AC_CONFIG_FILES([
    Makefile
    core/metrictools/Makefile
    core/libcplot/Makefile
    core/metrictools/storage/mysql/Makefile
    core/metrictools/storage/sqlite/Makefile
    test/Makefile])

AC_OUTPUT

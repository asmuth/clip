## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

lib_LIBRARIES = libtsdb.a
check_PROGRAMS= tsdb_smoketest
TESTS = tsdb_smoketest

################# DEPENDENCIES ####################

if HAVE_PTHREAD
PTHREAD_DEF = -DHAVE_PTHREAD=1
PTHREAD_LDFLAGS_=-lpthread
else
PTHREAD_DEF =
endif

AM_CXXFLAGS = -DFNORDMETRIC_VERSION=\"v@PACKAGE_VERSION@\" $(PTHREAD_CFLAGS) $(PTHREAD_DEF) -std=c++0x -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -g -I$(top_srcdir)/src/fnordmetric
AM_LDFLAGS = $(PTHREAD_CFLAGS)

libtsdb_a_SOURCES = \
    cursor.h \
    cursor.cc \
    op_commit.cc \
    op_insert.cc \
    page_buffer.h \
    page_buffer.cc \
    page_map.h \
    page_map.cc \
    page_index.h \
    page_index.cc \
    tsdb.h \
    tsdb.cc \
    transaction.h \
    transaction.cc \
    varint.h \
    varint.cc

tsdb_smoketest_SOURCES = \
    @top_srcdir@/src/fnordmetric/util/uri.h \
    @top_srcdir@/src/fnordmetric/util/uri.cc \
    @top_srcdir@/src/fnordmetric/util/time_impl.h \
    @top_srcdir@/src/fnordmetric/util/time.h \
    @top_srcdir@/src/fnordmetric/util/time.cc \
    @top_srcdir@/src/fnordmetric/util/stringutil_impl.h \
    @top_srcdir@/src/fnordmetric/util/stringutil.h \
    @top_srcdir@/src/fnordmetric/util/stringutil.cc \
    @top_srcdir@/src/fnordmetric/util/stdtypes.h \
    @top_srcdir@/src/fnordmetric/util/status.h \
    @top_srcdir@/src/fnordmetric/util/status.cc \
    @top_srcdir@/src/fnordmetric/util/stats/statssink.h \
    @top_srcdir@/src/fnordmetric/util/stats/statssink.cc \
    @top_srcdir@/src/fnordmetric/util/stats/statsrepository.h \
    @top_srcdir@/src/fnordmetric/util/stats/statsrepository.cc \
    @top_srcdir@/src/fnordmetric/util/stats/stats.h \
    @top_srcdir@/src/fnordmetric/util/stats/stat.h \
    @top_srcdir@/src/fnordmetric/util/stats/multicounter_impl.h \
    @top_srcdir@/src/fnordmetric/util/stats/multicounter.h \
    @top_srcdir@/src/fnordmetric/util/stats/movingwindow.h \
    @top_srcdir@/src/fnordmetric/util/stats/counterstat.h \
    @top_srcdir@/src/fnordmetric/util/stats/counter_impl.h \
    @top_srcdir@/src/fnordmetric/util/stats/counter.h \
    @top_srcdir@/src/fnordmetric/util/ieee754.h \
    @top_srcdir@/src/fnordmetric/util/ieee754.cc \
    @top_srcdir@/src/fnordmetric/util/sha1.h \
    @top_srcdir@/src/fnordmetric/util/sha1.cc \
    @top_srcdir@/src/fnordmetric/util/return_code.h \
    @top_srcdir@/src/fnordmetric/util/outputstream.h \
    @top_srcdir@/src/fnordmetric/util/outputstream.cc \
    @top_srcdir@/src/fnordmetric/util/option_impl.h \
    @top_srcdir@/src/fnordmetric/util/option.h \
    @top_srcdir@/src/fnordmetric/util/logging.h \
    @top_srcdir@/src/fnordmetric/util/logging.cc \
    @top_srcdir@/src/fnordmetric/util/inspect_impl.h \
    @top_srcdir@/src/fnordmetric/util/inspect.h \
    @top_srcdir@/src/fnordmetric/util/inspect.cc \
    @top_srcdir@/src/fnordmetric/util/inputstream.h \
    @top_srcdir@/src/fnordmetric/util/inputstream.cc \
    @top_srcdir@/src/fnordmetric/util/hash.h \
    @top_srcdir@/src/fnordmetric/util/flagparser.h \
    @top_srcdir@/src/fnordmetric/util/flagparser.cc \
    @top_srcdir@/src/fnordmetric/util/fileutil.h \
    @top_srcdir@/src/fnordmetric/util/fileutil.cc \
    @top_srcdir@/src/fnordmetric/util/file.h \
    @top_srcdir@/src/fnordmetric/util/file.cc \
    @top_srcdir@/src/fnordmetric/util/exception.h \
    @top_srcdir@/src/fnordmetric/util/exception.cc \
    @top_srcdir@/src/fnordmetric/util/bufferutil.h \
    @top_srcdir@/src/fnordmetric/util/bufferutil.cc \
    @top_srcdir@/src/fnordmetric/util/buffer.h \
    @top_srcdir@/src/fnordmetric/util/buffer.cc \
    @top_srcdir@/src/fnordmetric/util/base64.h \
    @top_srcdir@/src/fnordmetric/util/autoref_impl.h \
    @top_srcdir@/src/fnordmetric/util/autoref.h \
    @top_srcdir@/src/fnordmetric/util/autoref.cc \
    test/smoketest.cc

tsdb_smoketest_LDADD = libtsdb.a

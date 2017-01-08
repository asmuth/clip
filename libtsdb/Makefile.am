## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

lib_LIBRARIES = libtsdb.a
check_PROGRAMS= tsdb_smoketest
TESTS = tsdb_smoketest

################# DEPENDENCIES ####################

FEATURE_TEST_DEFS =

if HAVE_PTHREAD
FEATURE_TEST_DEFS += -DHAVE_PTHREAD=1
PTHREAD_LDFLAGS_=-lpthread
endif

if HAVE_POSIX_FALLOCATE
FEATURE_TEST_DEFS += -DHAVE_POSIX_FALLOCATE=1
endif

if HAVE_POSIX_FADVISE
FEATURE_TEST_DEFS += -DHAVE_POSIX_FADVISE=1
endif

if HAVE_FDATASYNC
FEATURE_TEST_DEFS += -DHAVE_FDATASYNC=1
endif

AM_CXXFLAGS = -DFNORDMETRIC_VERSION=\"v@PACKAGE_VERSION@\" $(FEATURE_TEST_DEFS) -std=c++0x -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -g -I$(top_srcdir)/ -I$(top_srcdir)/metricd/src
AM_LDFLAGS = $(PTHREAD_LDFLAGS)

libtsdb_a_SOURCES = \
    cursor.h \
    cursor.cc \
    op_commit.cc \
    op_insert.cc \
    op_load.cc \
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
    @top_srcdir@/metricd/src/metricd/util/time_impl.h \
    @top_srcdir@/metricd/src/metricd/util/time.h \
    @top_srcdir@/metricd/src/metricd/util/time.cc \
    @top_srcdir@/metricd/src/metricd/util/stringutil_impl.h \
    @top_srcdir@/metricd/src/metricd/util/stringutil.h \
    @top_srcdir@/metricd/src/metricd/util/stringutil.cc \
    @top_srcdir@/metricd/src/metricd/util/stdtypes.h \
    @top_srcdir@/metricd/src/metricd/util/status.h \
    @top_srcdir@/metricd/src/metricd/util/status.cc \
    @top_srcdir@/metricd/src/metricd/util/stats/statssink.h \
    @top_srcdir@/metricd/src/metricd/util/stats/statssink.cc \
    @top_srcdir@/metricd/src/metricd/util/stats/statsrepository.h \
    @top_srcdir@/metricd/src/metricd/util/stats/statsrepository.cc \
    @top_srcdir@/metricd/src/metricd/util/stats/stats.h \
    @top_srcdir@/metricd/src/metricd/util/stats/stat.h \
    @top_srcdir@/metricd/src/metricd/util/stats/multicounter_impl.h \
    @top_srcdir@/metricd/src/metricd/util/stats/multicounter.h \
    @top_srcdir@/metricd/src/metricd/util/stats/movingwindow.h \
    @top_srcdir@/metricd/src/metricd/util/stats/counterstat.h \
    @top_srcdir@/metricd/src/metricd/util/stats/counter_impl.h \
    @top_srcdir@/metricd/src/metricd/util/stats/counter.h \
    @top_srcdir@/metricd/src/metricd/util/ieee754.h \
    @top_srcdir@/metricd/src/metricd/util/ieee754.cc \
    @top_srcdir@/metricd/src/metricd/util/sha1.h \
    @top_srcdir@/metricd/src/metricd/util/sha1.cc \
    @top_srcdir@/metricd/src/metricd/util/return_code.h \
    @top_srcdir@/metricd/src/metricd/util/outputstream.h \
    @top_srcdir@/metricd/src/metricd/util/outputstream.cc \
    @top_srcdir@/metricd/src/metricd/util/option_impl.h \
    @top_srcdir@/metricd/src/metricd/util/option.h \
    @top_srcdir@/metricd/src/metricd/util/logging.h \
    @top_srcdir@/metricd/src/metricd/util/logging.cc \
    @top_srcdir@/metricd/src/metricd/util/inspect_impl.h \
    @top_srcdir@/metricd/src/metricd/util/inspect.h \
    @top_srcdir@/metricd/src/metricd/util/inspect.cc \
    @top_srcdir@/metricd/src/metricd/util/inputstream.h \
    @top_srcdir@/metricd/src/metricd/util/inputstream.cc \
    @top_srcdir@/metricd/src/metricd/util/hash.h \
    @top_srcdir@/metricd/src/metricd/util/flagparser.h \
    @top_srcdir@/metricd/src/metricd/util/flagparser.cc \
    @top_srcdir@/metricd/src/metricd/util/fileutil.h \
    @top_srcdir@/metricd/src/metricd/util/fileutil.cc \
    @top_srcdir@/metricd/src/metricd/util/file.h \
    @top_srcdir@/metricd/src/metricd/util/file.cc \
    @top_srcdir@/metricd/src/metricd/util/exception.h \
    @top_srcdir@/metricd/src/metricd/util/exception.cc \
    @top_srcdir@/metricd/src/metricd/util/bufferutil.h \
    @top_srcdir@/metricd/src/metricd/util/bufferutil.cc \
    @top_srcdir@/metricd/src/metricd/util/buffer.h \
    @top_srcdir@/metricd/src/metricd/util/buffer.cc \
    @top_srcdir@/metricd/src/metricd/util/base64.h \
    @top_srcdir@/metricd/src/metricd/util/autoref_impl.h \
    @top_srcdir@/metricd/src/metricd/util/autoref.h \
    @top_srcdir@/metricd/src/metricd/util/autoref.cc \
    test/smoketest.cc

tsdb_smoketest_LDADD = libtsdb.a

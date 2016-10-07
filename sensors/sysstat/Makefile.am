## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

bin_PROGRAMS = sensor_sysstat

################# DEPENDENCIES ####################

if HAVE_PTHREAD
PTHREAD_DEF = -DHAVE_PTHREAD=1
PTHREAD_LDFLAGS_=-lpthread
else
PTHREAD_DEF =
endif

AM_CXXFLAGS = -DFNORDMETRIC_VERSION=\"v@PACKAGE_VERSION@\" $(PTHREAD_CFLAGS) $(PTHREAD_DEF) -std=c++0x -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -g -I$(top_srcdir)
AM_LDFLAGS = $(PTHREAD_LDFLAGS_)

sensor_sysstat_SOURCES = \
    @top_srcdir@/metricd/util/uri.h \
    @top_srcdir@/metricd/util/uri.cc \
    @top_srcdir@/metricd/util/time_impl.h \
    @top_srcdir@/metricd/util/time.h \
    @top_srcdir@/metricd/util/time.cc \
    @top_srcdir@/metricd/util/stringutil_impl.h \
    @top_srcdir@/metricd/util/stringutil.h \
    @top_srcdir@/metricd/util/stringutil.cc \
    @top_srcdir@/metricd/util/stdtypes.h \
    @top_srcdir@/metricd/util/status.h \
    @top_srcdir@/metricd/util/status.cc \
    @top_srcdir@/metricd/util/stats/statssink.h \
    @top_srcdir@/metricd/util/stats/statssink.cc \
    @top_srcdir@/metricd/util/stats/statsrepository.h \
    @top_srcdir@/metricd/util/stats/statsrepository.cc \
    @top_srcdir@/metricd/util/stats/stats.h \
    @top_srcdir@/metricd/util/stats/stat.h \
    @top_srcdir@/metricd/util/stats/multicounter_impl.h \
    @top_srcdir@/metricd/util/stats/multicounter.h \
    @top_srcdir@/metricd/util/stats/movingwindow.h \
    @top_srcdir@/metricd/util/stats/counterstat.h \
    @top_srcdir@/metricd/util/stats/counter_impl.h \
    @top_srcdir@/metricd/util/stats/counter.h \
    @top_srcdir@/metricd/util/ieee754.h \
    @top_srcdir@/metricd/util/ieee754.cc \
    @top_srcdir@/metricd/util/sha1.h \
    @top_srcdir@/metricd/util/sha1.cc \
    @top_srcdir@/metricd/util/return_code.h \
    @top_srcdir@/metricd/util/outputstream.h \
    @top_srcdir@/metricd/util/outputstream.cc \
    @top_srcdir@/metricd/util/option_impl.h \
    @top_srcdir@/metricd/util/option.h \
    @top_srcdir@/metricd/util/logging.h \
    @top_srcdir@/metricd/util/logging.cc \
    @top_srcdir@/metricd/util/inspect_impl.h \
    @top_srcdir@/metricd/util/inspect.h \
    @top_srcdir@/metricd/util/inspect.cc \
    @top_srcdir@/metricd/util/inputstream.h \
    @top_srcdir@/metricd/util/inputstream.cc \
    @top_srcdir@/metricd/util/hash.h \
    @top_srcdir@/metricd/util/flagparser.h \
    @top_srcdir@/metricd/util/flagparser.cc \
    @top_srcdir@/metricd/util/fileutil.h \
    @top_srcdir@/metricd/util/fileutil.cc \
    @top_srcdir@/metricd/util/file.h \
    @top_srcdir@/metricd/util/file.cc \
    @top_srcdir@/metricd/util/exception.h \
    @top_srcdir@/metricd/util/exception.cc \
    @top_srcdir@/metricd/util/bufferutil.h \
    @top_srcdir@/metricd/util/bufferutil.cc \
    @top_srcdir@/metricd/util/buffer.h \
    @top_srcdir@/metricd/util/buffer.cc \
    @top_srcdir@/metricd/util/base64.h \
    @top_srcdir@/metricd/util/autoref_impl.h \
    @top_srcdir@/metricd/util/autoref.h \
    @top_srcdir@/metricd/util/autoref.cc \
    sensor_sysstat.cc


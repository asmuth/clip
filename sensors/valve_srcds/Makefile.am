## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

bin_PROGRAMS = metricd-sensor-valve_srcds

################# DEPENDENCIES ####################

if HAVE_PTHREAD
PTHREAD_DEF = -DHAVE_PTHREAD=1
PTHREAD_LDFLAGS_=-lpthread
else
PTHREAD_DEF =
endif

AM_CXXFLAGS = -DFNORDMETRIC_VERSION=\"v@PACKAGE_VERSION@\" $(PTHREAD_CFLAGS) $(PTHREAD_DEF) -std=c++0x -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -g -I$(top_srcdir)/metricd/src -I$(top_srcdir)
AM_LDFLAGS = $(PTHREAD_LDFLAGS_)

metricd_sensor_valve_srcds_SOURCES = \
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
    @top_srcdir@/metricd/src/metricd/transport/statsd/statsd.h \
    @top_srcdir@/metricd/src/metricd/transport/statsd/statsd.cc \
    srcds_client.h \
    srcds_client.cc \
    sensor_valve_srcds.cc


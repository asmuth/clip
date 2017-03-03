## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

bin_PROGRAMS = metric-collectd metric-queryd

################# DEPENDENCIES ####################
if HAVE_ZLIB
ZLIB_DEF = -DHAVE_ZLIB=1
else
ZLIB_DEF =
endif

PTHREAD_DEF = -DHAVE_PTHREAD=1
PTHREAD_LDFLAGS_=-lpthread

if HAVE_SYSLOG_H
SYSLOG_DEF = -DHAVE_SYSLOG_H=1
else
SYSLOG_DEF =
endif

if HAVE_GETHOSTBYNAME_R
GETHOSTBYNAME_R_DEF = -DHAVE_GETHOSTBYNAME_R=1
else
GETHOSTBYNAME_R_DEF =
endif

PCRE_LDFLAGS_=-lpcre
CURL_LDFLAGS_=-lcurl

AM_CXXFLAGS = -DFNORDMETRIC_VERSION=\"v@PACKAGE_VERSION@\" $(PTHREAD_CFLAGS) $(PTHREAD_DEF) $(SYSLOG_DEF) $(ZLIB_DEF) $(GETHOSTBYNAME_R_DEF) -std=c++0x -ftemplate-depth=500 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -Wno-invalid-offsetof -g -I$(top_srcdir)/src -I$(top_srcdir)/deps/libtransport/src -I$(top_srcdir)/
AM_CFLAGS =  $(PTHREAD_CFLAGS) $(PTHREAD_DEF) $(SYSLOG_DEF) $(ZLIB_DEF) $(GETHOSTBYNAME_R_DEF) -std=c11 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -pedantic -g

fnordmetric_util_src = \
    util/daemonize.h \
    util/daemonize.cc \
    util/time_impl.h \
    util/time.h \
    util/time.cc \
    util/format.h \
    util/format.cc \
    util/stringutil_impl.h \
    util/stringutil.h \
    util/stringutil.cc \
    util/stdtypes.h \
    util/status.h \
    util/status.cc \
    util/stats/statssink.h \
    util/stats/statssink.cc \
    util/stats/statsrepository.h \
    util/stats/statsrepository.cc \
    util/stats/stats.h \
    util/stats/stat.h \
    util/stats/multicounter_impl.h \
    util/stats/multicounter.h \
    util/stats/movingwindow.h \
    util/stats/counterstat.h \
    util/stats/counter_impl.h \
    util/stats/counter.h \
    util/ieee754.h \
    util/ieee754.cc \
    util/sha1.h \
    util/sha1.cc \
    util/return_code.h \
    util/outputstream.h \
    util/outputstream.cc \
    util/option_impl.h \
    util/option.h \
    util/logging.h \
    util/logging.cc \
    util/inspect_impl.h \
    util/inspect.h \
    util/inspect.cc \
    util/inputstream.h \
    util/inputstream.cc \
    util/hash.h \
    util/flagparser.h \
    util/flagparser.cc \
    util/fileutil.h \
    util/fileutil.cc \
    util/file.h \
    util/file.cc \
    util/exception.h \
    util/exception.cc \
    util/bufferutil.h \
    util/bufferutil.cc \
    util/buffer.h \
    util/buffer.cc \
    util/base64.h \
    util/autoref_impl.h \
    util/autoref.h \
    util/autoref.cc

metric_collectd_SOURCES = \
    aggregate.h \
    aggregate.cc \
    aggregation_service.h \
    aggregation_service.cc \
    config_list.h \
    config_list.cc \
    config_parser.h \
    config_parser.cc \
    table.h \
    table.cc \
    table_map.h \
    table_map.cc \
    sample.h \
    sample.cc \
    sensors.h \
    sensors.cc \
    units.h \
    units.cc \
    transport/statsd/statsd.h \
    transport/statsd/statsd.cc \
    transport/statsd/statsd_server.h \
    transport/statsd/statsd_server.cc \
    types.h \
    types.cc \
    $(fnordmetric_util_src) \
    metric_collectd.cc

metric_collectd_LDADD = \
    ${top_builddir}/deps/libtransport/src/libtransport/http/libtransport_http.a \
    ${top_builddir}/deps/libtransport/src/libtransport/json/libtransport_json.a \
    ${top_builddir}/deps/libtransport/src/libtransport/uri/libtransport_uri.a \
    $(PTHREAD_LDFLAGS_)

metric_queryd_SOURCES = \
    $(fnordmetric_util_src) \
    metric_queryd.cc

metric_queryd_LDADD = \
    ${top_builddir}/deps/libtransport/src/libtransport/http/libtransport_http.a \
    ${top_builddir}/deps/libtransport/src/libtransport/json/libtransport_json.a \
    ${top_builddir}/deps/libtransport/src/libtransport/uri/libtransport_uri.a \
    $(PTHREAD_LDFLAGS_)

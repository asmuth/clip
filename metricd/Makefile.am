## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

bin_PROGRAMS = metricd
TESTS = metric_test statsd_test
check_PROGRAMS = $(TESTS)


################# DEPENDENCIES ####################
if HAVE_ZLIB
ZLIB_DEF = -DHAVE_ZLIB=1
else
ZLIB_DEF =
endif

if HAVE_PTHREAD
PTHREAD_DEF = -DHAVE_PTHREAD=1
PTHREAD_LDFLAGS_=-lpthread
else
PTHREAD_DEF =
endif

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

AM_CXXFLAGS = -DFNORDMETRIC_VERSION=\"v@PACKAGE_VERSION@\" $(PTHREAD_CFLAGS) $(PTHREAD_DEF) $(SYSLOG_DEF) $(ZLIB_DEF) $(GETHOSTBYNAME_R_DEF) -std=c++0x -ftemplate-depth=500 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -Wno-invalid-offsetof -g -I$(top_srcdir)/ -I$(top_builddir)/
AM_CFLAGS =  $(PTHREAD_CFLAGS) $(PTHREAD_DEF) $(SYSLOG_DEF) $(ZLIB_DEF) $(GETHOSTBYNAME_R_DEF) -std=c11 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -pedantic -g
AM_LDFLAGS = $(PCRE_LDFLAGS_) $(CURL_LDFLAGS_) $(PTHREAD_LDFLAGS_)

metricd_common_sources_ = \
    metric.h \
    metric.cc \
    metric_map.h \
    metric_map.cc \
    metric_service.h \
    metric_service.cc \
    sample.h \
    sample.cc \
    util/json.h \
    util/json.cc \
    util/uri.h \
    util/uri.cc \
    util/time_impl.h \
    util/time.h \
    util/time.cc \
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
    util/autoref.cc \
    transport/http/httpapi.h \
    transport/http/httpapi.cc \
    transport/http/wakeup.h \
    transport/http/wakeup.cc \
    transport/http/tcpserver.h \
    transport/http/tcpserver.cc \
    transport/http/tcpconnection.h \
    transport/http/tcpconnection.cc \
    transport/http/taskscheduler.h \
    transport/http/task.h \
    transport/http/status.h \
    transport/http/inetaddr.h \
    transport/http/inetaddr.cc \
    transport/http/httpstats.h \
    transport/http/HTTPSSEStream.h \
    transport/http/HTTPSSEStream.cc \
    transport/http/httpservice.h \
    transport/http/httpservice.cc \
    transport/http/httpserverconnection.h \
    transport/http/httpserverconnection.cc \
    transport/http/httpserver.h \
    transport/http/httpserver.cc \
    transport/http/httprouter_impl.h \
    transport/http/httprouter.h \
    transport/http/httprouter.cc \
    transport/http/HTTPResponseStream.h \
    transport/http/HTTPResponseStream.cc \
    transport/http/httpresponsehandler.h \
    transport/http/httpresponse.h \
    transport/http/httpresponse.cc \
    transport/http/HTTPRequestStream.h \
    transport/http/HTTPRequestStream.cc \
    transport/http/httprequest.h \
    transport/http/httprequest.cc \
    transport/http/httpparser.h \
    transport/http/httpparser.cc \
    transport/http/httpmessage.h \
    transport/http/httpmessage.cc \
    transport/http/httphandler.h \
    transport/http/httpgenerator.h \
    transport/http/httpgenerator.cc \
    transport/http/httpconnectionhandler.h \
    transport/http/eventloop.h \
    transport/http/eventloop.cc \
    transport/http/cookies.h \
    transport/http/cookies.cc \
    transport/statsd/statsd.h \
    transport/statsd/statsd.cc \
    webui/webui.h \
    webui/webui.cc

metricd_SOURCES = \
    $(metricd_common_sources_) \
    metricd.cc

metricd_LDADD = $(top_srcdir)/libtsdb/libtsdb.a

metric_test_SOURCES = \
    $(metricd_common_sources_) \
    metric_test.cc

metric_test_LDADD = $(top_srcdir)/libtsdb/libtsdb.a

statsd_test_SOURCES = \
    $(metricd_common_sources_) \
    transport/statsd/statsd_test.cc

statsd_test_LDADD = $(top_srcdir)/libtsdb/libtsdb.a

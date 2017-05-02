bin_PROGRAMS = metric-collectd metric-queryd metricctl
noinst_LIBRARIES = libfnordmetric-common.a
SUBDIRS = backends/mysql

libfnordmetric_common_a_SOURCES = \
    backends/backend.h \
    backends/backend.cc \
    config_list.h \
    config_list.cc \
    config_parser.h \
    config_parser.cc \
    cli/command.h \
    cli/commands/insert_cmd.h \
    cli/commands/insert_cmd.cc \
    sample.h \
    sample.cc \
    ingest.h \
    ingest.cc \
    units.h \
    units.cc \
    statsd.h \
    statsd.cc \
    listen_udp.h \
    listen_udp.cc \
    listen_http.h \
    listen_http.cc \
    metric.h \
    metric.cc \
    metric_map.h \
    metric_map.cc \
    fetch_http.h \
    fetch_http.cc \
    query_service.h \
    query_service.cc \
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
    util/autoref.cc \
    storage/op.h \
    storage/ops/insert_op.h \
    storage/ops/insert_op.cc

FNORDMETRIC_LIBS = \
    libfnordmetric-common.a \
    -L$(abs_top_builddir)/deps/libtransport/src/libtransport/http \
    -L$(abs_top_builddir)/deps/libtransport/src/libtransport/json \
    -L$(abs_top_builddir)/deps/libtransport/src/libtransport/uri \
    -ltransport_http \
    -ltransport_json \
    -ltransport_uri

if ENABLE_MYSQL
  FNORDMETRIC_LIBS += backends/mysql/libfnordmetric-backend-mysql.a
  FNORDMETRIC_LIBS += -lmysqlclient
endif

FNORDMETRIC_LIBS += libfnordmetric-common.a

metric_collectd_SOURCES = metric_collectd.cc
metric_collectd_LDADD = $(FNORDMETRIC_LIBS)

metric_queryd_SOURCES = metric_queryd.cc
metric_queryd_LDADD = $(FNORDMETRIC_LIBS)

metricctl_SOURCES = metricctl.cc
metricctl_LDADD = $(FNORDMETRIC_LIBS)

bin_PROGRAMS = metrictl
noinst_LIBRARIES = libfnordmetric-common.a
SUBDIRS = storage/mysql storage/sqlite

libfnordmetric_common_a_SOURCES = \
    config_list.h \
    config_list.cc \
    config_parser.h \
    config_parser.cc \
    cli/command.h \
    cli/commands/collect_cmd.h \
    cli/commands/collect_cmd.cc \
    cli/commands/insert_cmd.h \
    cli/commands/insert_cmd.cc \
    cli/commands/list_cmd.h \
    cli/commands/list_cmd.cc \
    cli/commands/log_cmd.h \
    cli/commands/log_cmd.cc \
    cli/commands/plot_cmd.h \
    cli/commands/plot_cmd.cc \
    collect.h \
    collect.cc \
    collect_http.h \
    collect_http.cc \
    collect_proc.h \
    collect_proc.cc \
    measure.h \
    measure.cc \
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
    plot.h \
    plot.cc \
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
    storage/backend.h \
    storage/backend.cc \
    storage/op.h \
    storage/ops/fetch_op.h \
    storage/ops/fetch_op.cc \
    storage/ops/insert_op.h \
    storage/ops/insert_op.cc \
    timeseries.h \
    timeseries_impl.h

FNORDMETRIC_LIBS = \
    libfnordmetric-common.a \
    -L$(abs_top_builddir)/deps/libtransport/src/libtransport/http \
    -L$(abs_top_builddir)/deps/libtransport/src/libtransport/json \
    -L$(abs_top_builddir)/deps/libtransport/src/libtransport/uri \
    -ltransport_http \
    -ltransport_json \
    -ltransport_uri

if ENABLE_MYSQL
  FNORDMETRIC_LIBS += storage/mysql/libfnordmetric-backend-mysql.a
  FNORDMETRIC_LIBS += -lmysqlclient
endif

if ENABLE_SQLITE
  FNORDMETRIC_LIBS += storage/sqlite/sqlite_backend.a
  FNORDMETRIC_LIBS += -lsqlite3
endif

FNORDMETRIC_LIBS += libfnordmetric-common.a

metrictl_SOURCES = metrictl.cc
metrictl_LDADD = $(FNORDMETRIC_LIBS)

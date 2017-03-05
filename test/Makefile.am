TESTS = test-config_parser test-aggregation_map
check_PROGRAMS = $(TESTS)

FNORDMETRIC_TEST_LIBS = \
    ../src/fnordmetric/libfnordmetric-common.a \
    -L$(abs_top_builddir)/deps/libtransport/src/libtransport/http/libtransport_http.a \
    -L$(abs_top_builddir)/deps/libtransport/src/libtransport/json/libtransport_json.a \
    -L$(abs_top_builddir)/deps/libtransport/src/libtransport/uri \
    -ltransport_http \
    -ltransport_json \
    -ltransport_uri \
    ../src/fnordmetric/libfnordmetric-common.a

test_config_parser_SOURCES = config_parser_test.cc
test_config_parser_LDADD = $(FNORDMETRIC_TEST_LIBS)

test_aggregation_map_SOURCES = aggregation_map_test.cc
test_aggregation_map_LDADD = $(FNORDMETRIC_TEST_LIBS)


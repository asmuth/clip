## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

TESTS = test-config_parser test-aggregation_map
check_PROGRAMS = $(TESTS)

AM_CXXFLAGS = -std=c++0x -ftemplate-depth=500 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -Wno-invalid-offsetof -g -I$(top_srcdir)/src -I$(top_srcdir)/deps/libtransport/src -I$(top_srcdir)/
AM_CFLAGS = -std=c11 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -pedantic -g
AM_LDFLAGS = -pthread

test_config_parser_SOURCES = config_parser_test.cc
test_config_parser_LDADD = ${top_builddir}/src/fnordmetric/libfnordmetric-common.a

test_aggregation_map_SOURCES = aggregation_map_test.cc
test_aggregation_map_LDADD = ${top_builddir}/src/fnordmetric/libfnordmetric-common.a

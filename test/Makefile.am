## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

TESTS = config_parser_test
check_PROGRAMS = $(TESTS)

AM_CXXFLAGS = -std=c++0x -ftemplate-depth=500 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -Wno-invalid-offsetof -g -I$(top_srcdir)/src -I$(top_srcdir)/../
AM_CFLAGS = -std=c11 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -pedantic -g
AM_LDFLAGS = -pthread

config_parser_test_SOURCES = config_parser_test.cc
config_parser_test_LDADD = ${top_builddir}/src/fnordmetric/libfnordmetric-common.a

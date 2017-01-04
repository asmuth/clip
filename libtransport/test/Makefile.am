## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

TESTS = http_test uri_test
check_PROGRAMS = $(TESTS)

AM_CXXFLAGS = -std=c++0x -ftemplate-depth=500 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -Wno-invalid-offsetof -g -I$(top_srcdir)/src
AM_CFLAGS = -std=c11 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -pedantic -g
AM_LDFLAGS = -pthread

http_test_SOURCES = http_test.cc
http_test_LDADD = \
    ${top_builddir}/src/libtransport/http/libtransport_http.a \
    ${top_builddir}/src/libtransport/uri/libtransport_uri.a

uri_test_SOURCES = uri_test.cc
uri_test_LDADD = ${top_builddir}/src/libtransport/uri/libtransport_uri.a


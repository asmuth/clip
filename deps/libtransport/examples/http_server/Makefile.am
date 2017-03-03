## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

bin_PROGRAMS = http_server_example

AM_CXXFLAGS = -std=c++0x -ftemplate-depth=500 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -Wno-invalid-offsetof -g -I$(top_srcdir)/src
AM_CFLAGS = -std=c11 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -pedantic -g
AM_LDFLAGS = -pthread

http_server_example_SOURCES = http_server_example.cc
http_server_example_LDADD = \
    ${top_builddir}/src/libtransport/http/libtransport_http.a \
    ${top_builddir}/src/libtransport/uri/libtransport_uri.a

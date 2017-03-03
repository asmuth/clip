## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

lib_LIBRARIES = libtransport_json.a

AM_CXXFLAGS = -DLIBTRANSPORT_VERSION=\"v@PACKAGE_VERSION@\" -std=c++0x -ftemplate-depth=500 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -Wno-invalid-offsetof -g -I$(top_srcdir)/src
AM_CFLAGS = -std=c11 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -pedantic -g

libtransport_json_a_SOURCES = \
    json.h \
    json.cc \
    json_object.h \
    json_object.cc \
    json_reader.h \
    json_reader.cc \
    json_writer.h \
    json_writer.cc

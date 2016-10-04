## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

lib_LIBRARIES = libtsdb.a

################# DEPENDENCIES ####################

if HAVE_PTHREAD
PTHREAD_DEF = -DHAVE_PTHREAD=1
PTHREAD_LDFLAGS_=-lpthread
else
PTHREAD_DEF =
endif

AM_CXXFLAGS = -DFNORDMETRIC_VERSION=\"v@PACKAGE_VERSION@\" $(PTHREAD_CFLAGS) $(PTHREAD_DEF) -std=c++0x -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -g -I$(top_srcdir)/src/fnordmetric
AM_LDFLAGS = $(PTHREAD_CFLAGS)
AM_LDADD = $(PTHREAD_LDFLAGS_)

libtsdb_a_SOURCES = \
    transaction.h \
    transaction.cc

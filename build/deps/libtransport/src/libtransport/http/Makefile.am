## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

lib_LIBRARIES = libtransport_http.a

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

if HAVE_GETHOSTBYNAME_R
GETHOSTBYNAME_R_DEF = -DHAVE_GETHOSTBYNAME_R=1
else
GETHOSTBYNAME_R_DEF =
endif

AM_CXXFLAGS = -DLIBTRANSPORT_VERSION=\"v@PACKAGE_VERSION@\" $(PTHREAD_CFLAGS) $(PTHREAD_DEF)  $(ZLIB_DEF) $(GETHOSTBYNAME_R_DEF) -std=c++0x -ftemplate-depth=500 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -Wno-invalid-offsetof -g -I$(top_srcdir)/src
AM_CFLAGS =  $(PTHREAD_CFLAGS) $(PTHREAD_DEF) $(ZLIB_DEF) $(GETHOSTBYNAME_R_DEF) -std=c11 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -pedantic -g
AM_LDFLAGS = $(PCRE_LDFLAGS_) $(CURL_LDFLAGS_) $(PTHREAD_LDFLAGS_)


libtransport_http_a_SOURCES = \
    cookies.cc \
    cookies.h \
    http_message.cc \
    http_message.h \
    http_request.cc \
    http_request.h \
    http_response.cc \
    http_response.h \
    http_status.h \
    sse_generator.cc \
    sse_generator.h \
    sse_parser.cc \
    sse_parser.h \
    v1/http_client.cc \
    v1/http_client.h \
    v1/http_connection.cc \
    v1/http_connection.h \
    v1/http_generator.cc \
    v1/http_generator.h \
    v1/http_parser.cc \
    v1/http_parser.h \
    v1/http_server.cc \
    v1/http_server.h

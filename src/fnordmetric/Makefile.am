## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES =   \
  Makefile.in

# we'll fill those later
bin_PROGRAMS =
noinst_LIBRARIES =
dist_noinst_DATA =
BUILT_SOURCES =
TESTS =
check_PROGRAMS =


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

if HAVE_SYSLOG_H
SYSLOG_DEF = -DHAVE_SYSLOG_H=1
else
SYSLOG_DEF =
endif

if HAVE_GETHOSTBYNAME_R
GETHOSTBYNAME_R_DEF = -DHAVE_GETHOSTBYNAME_R=1
else
GETHOSTBYNAME_R_DEF =
endif

PCRE_LDFLAGS_=-lpcre
CURL_LDFLAGS_=-lcurl

AM_CXXFLAGS = -DFNORDMETRIC_VERSION=\"v@PACKAGE_VERSION@\" $(PTHREAD_CFLAGS) $(PTHREAD_DEF) $(SYSLOG_DEF) $(ZLIB_DEF) $(GETHOSTBYNAME_R_DEF) -std=c++0x -ftemplate-depth=500 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wdelete-non-virtual-dtor -Wno-predefined-identifier-outside-function -Wno-invalid-offsetof -g -I$(top_srcdir)/src -I$(top_builddir)/src
AM_CFLAGS =  $(PTHREAD_CFLAGS) $(PTHREAD_DEF) $(SYSLOG_DEF) $(ZLIB_DEF) $(GETHOSTBYNAME_R_DEF) -std=c11 -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer -Wall -pedantic -g
AM_LDFLAGS = $(PTHREAD_CFLAGS)
AM_LDADD = $(PCRE_LDFLAGS_) $(CURL_LDFLAGS_) $(PTHREAD_LDFLAGS_)

####### FMETRICD ==############################################################

fmetricd_SOURCES = \
		util/autoref.h \
		util/autoref_impl.h \
		util/autoref.cc \
		util/logging.h \
		util/logging.cc \
		util/inspect.h \
		util/inspect_impl.h \
		util/inspect.cc \
		util/flagparser.h \
		util/flagparser.cc \
		util/return_code.h \
		util/stringutil.h \
		util/stringutil_impl.h \
		util/stringutil.cc \
		util/time.h \
		util/time_impl.h \
		util/time.cc \
		util/sha1.h \
		util/sha1.cc \
		util/base64.h \
		util/exception.h \
		util/stdtypes.h \
		transport/http/eventloop.h \
		transport/http/eventloop.cc \
		transport/http/wakeup.h \
		transport/http/wakeup.cc \
		transport/http/HTTPRequestStream.cc \
		transport/http/HTTPRequestStream.h \
		transport/http/HTTPResponseStream.cc \
		transport/http/HTTPResponseStream.h \
		transport/http/httpconnectionhandler.h \
		transport/http/HTTPSSEStream.cc \
		transport/http/HTTPSSEStream.h \
		transport/http/cookies.cc \
		transport/http/cookies.h \
		transport/http/status.h \
		transport/http/httphandler.h \
		transport/http/httpgenerator.cc \
		transport/http/httpgenerator.h \
		transport/http/httpmessage.cc \
		transport/http/httpmessage.h \
		transport/http/httpparser.cc \
		transport/http/httpparser.h \
		transport/http/httprequest.cc \
		transport/http/httprequest.h \
		transport/http/httpresponse.cc \
		transport/http/httpresponse.h \
		transport/http/httpresponsehandler.h \
		transport/http/httprouter.cc \
		transport/http/httprouter.h \
		transport/http/httprouter_impl.h \
		transport/http/httpserver.cc \
		transport/http/httpserver.h \
		transport/http/httpserverconnection.cc \
		transport/http/httpserverconnection.h \
		transport/http/httpstats.h \
		transport/http/httpservice.cc \
		transport/http/httpservice.h \
		fmetricd.cc

bin_PROGRAMS += fmetricd
fmetricd_LDADD = ${AM_LDADD}

####### TESTS #################################################################

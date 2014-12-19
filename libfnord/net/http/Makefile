SHELL          = /bin/sh
CXX           ?= clang++
CXXFLAGS       = -I../../.. -std=c++11
LDFLAGS        =

UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)
CXXFLAGS += -stdlib=libc++
endif

FNORD_HTTP_TARGET  = libfnord-http.a
FNORD_HTTP_SOURCES = cookies.cc httpconnection.cc httpmessage.cc httpparser.cc httpgenerator.cc httprequest.cc httpresponse.cc httpserver.cc httprouter.cc
FNORD_HTTP_OBJECTS = $(subst .cc,.o,$(FNORD_HTTP_SOURCES))

%.o : %.cc
	$(CXX) -fPIC -c $(CFLAGS) $(CXXFLAGS) -c $<

$(FNORD_HTTP_TARGET): $(FNORD_HTTP_OBJECTS)
	ar rcs $(FNORD_HTTP_TARGET) $(FNORD_HTTP_OBJECTS)

http_test: libfnord-http.a http_test.cc
	(cd ../../base && make)
	(cd ../../io && make)
	(cd ../../logging && make)
	(cd ../../system && make)
	(cd ../../thread && make)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o http_test http_test.cc -L. -L../../base -L../../io -lfnord-http -lfnord-io -lfnord-base -lpthread

http_test\:run: http_test
	./http_test

httpserver_example: libfnord-http.a httpserver_example.cc
	(cd ../../base && make)
	(cd ../../io && make)
	(cd ../../logging && make)
	(cd ../../system && make)
	(cd ../../thread && make)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o httpserver_example httpserver_example.cc -L. -L../../base -L../../io -L../../logging -L../../net -L../../system -L../../thread -lfnord-http -lfnord-io -lfnord-base -lfnord-system -lfnord-thread -lfnord-logging -lfnord-net -lpthread

httpserver_example\:run: httpserver_example
	./httpserver_example

clean:
	rm -f ${FNORD_HTTP_OBJECTS} $(FNORD_HTTP_TARGET)

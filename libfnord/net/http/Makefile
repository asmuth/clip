SHELL          = /bin/sh
CXX           ?= clang++
CXXFLAGS       = -stdlib=libc++ -I../../.. -std=c++11
LDFLAGS        =

FNORD_HTTP_TARGET  = libfnord-http.a
FNORD_HTTP_SOURCES = cookies.cc httpinputstream.cc httpmessage.cc httpoutputstream.cc httprequest.cc httpresponse.cc httpserver.cc
FNORD_HTTP_OBJECTS = $(subst .cc,.o,$(FNORD_HTTP_SOURCES))

%.o : %.cc
	$(CXX) -fPIC -c $(CFLAGS) $(CXXFLAGS) -c $<

$(FNORD_HTTP_TARGET): $(FNORD_HTTP_OBJECTS)
	ar rcs $(FNORD_HTTP_TARGET) $(FNORD_HTTP_OBJECTS)

http_test: libfnord-http.a http_test.cc
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o http_test http_test.cc -L. -L../../base -L../../io -lfnord-http -lfnord-io -lfnord-base -lpthread

http_test\:run: http_test
	./http_test

clean:
	rm -f ${FNORD_HTTP_OBJECTS} $(FNORD_HTTP_TARGET)

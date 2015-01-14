SHELL          = /bin/sh
CXX           ?= clang++
CXXFLAGS       = -I../.. -std=c++11
LDFLAGS        =

UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)
CXXFLAGS += -stdlib=libc++
endif

FNORD_BASE_TARGET  = libfnord-base.a
FNORD_BASE_SOURCES = assets.cc buffer.cc bufferutil.cc datetime.cc exception.cc exceptionhandler.cc inspect.cc ieee754.cc random.cc stringutil.cc uri.cc wallclock.cc fnv.cc
FNORD_BASE_OBJECTS = $(subst .cc,.o,$(FNORD_BASE_SOURCES))

%.o : %.cc
	$(CXX) -fPIC -c $(CFLAGS) $(CXXFLAGS) -c $<

$(FNORD_BASE_TARGET): $(FNORD_BASE_OBJECTS)
	ar rcs $(FNORD_BASE_TARGET) $(FNORD_BASE_OBJECTS)

stringutil_test: $(FNORD_BASE_TARGET) stringutil_test.cc
	(cd ../io && make libfnord-io.a)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o stringutil_test stringutil_test.cc -L. -L../io -lfnord-base -lfnord-io -lpthread

stringutil_test\:run: stringutil_test
	./stringutil_test

clean:
	rm -f ${FNORD_BASE_OBJECTS} $(FNORD_BASE_TARGET)

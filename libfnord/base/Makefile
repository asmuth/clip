SHELL          = /bin/sh
CXX           ?= clang++
CXXFLAGS       = -stdlib=libc++ -I../.. -std=c++11
LDFLAGS        =

FNORD_BASE_TARGET  = libfnord-base.a
FNORD_BASE_SOURCES = assets.cc buffer.cc datetime.cc exception.cc exceptionhandler.cc inspect.cc ieee754.cc random.cc stringutil.cc uri.cc wallclock.cc fnv.cc
FNORD_BASE_OBJECTS = $(subst .cc,.o,$(FNORD_BASE_SOURCES))

%.o : %.cc
	$(CXX) -fPIC -c $(CFLAGS) $(CXXFLAGS) -c $<

$(FNORD_BASE_TARGET): $(FNORD_BASE_OBJECTS)
	ar rcs $(FNORD_BASE_TARGET) $(FNORD_BASE_OBJECTS)

clean:
	rm -f ${FNORD_BASE_OBJECTS} $(FNORD_BASE_TARGET)

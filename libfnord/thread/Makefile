SHELL          = /bin/sh
CXX           ?= clang++
CXXFLAGS       = -stdlib=libc++ -I../.. -std=c++11
LDFLAGS        =

FNORD_THREAD_TARGET  = libfnord-thread.a
FNORD_THREAD_SOURCES = threadpool.cc
FNORD_THREAD_OBJECTS = $(subst .cc,.o,$(FNORD_THREAD_SOURCES))

%.o : %.cc
	$(CXX) -fPIC -c $(CFLAGS) $(CXXFLAGS) -c $<

$(FNORD_THREAD_TARGET): $(FNORD_THREAD_OBJECTS)
	ar rcs $(FNORD_THREAD_TARGET) $(FNORD_THREAD_OBJECTS)

clean:
	rm -f ${FNORD_THREAD_OBJECTS} $(FNORD_THREAD_TARGET)

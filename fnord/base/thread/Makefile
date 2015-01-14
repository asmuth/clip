SHELL          = /bin/sh
CXX           ?= clang++
CXXFLAGS       = -I../.. -std=c++11
LDFLAGS        =

UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)
CXXFLAGS += -stdlib=libc++
endif

FNORD_THREAD_TARGET  = libfnord-thread.a
FNORD_THREAD_SOURCES = threadpool.cc eventloop.cc
FNORD_THREAD_OBJECTS = $(subst .cc,.o,$(FNORD_THREAD_SOURCES))

%.o : %.cc
	$(CXX) -fPIC -c $(CFLAGS) $(CXXFLAGS) -c $<

$(FNORD_THREAD_TARGET): $(FNORD_THREAD_OBJECTS)
	ar rcs $(FNORD_THREAD_TARGET) $(FNORD_THREAD_OBJECTS)

clean:
	rm -f ${FNORD_THREAD_OBJECTS} $(FNORD_THREAD_TARGET)

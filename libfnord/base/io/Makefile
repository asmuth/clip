SHELL          = /bin/sh
CXX           ?= clang++
CXXFLAGS       = -I../.. -std=c++11
LDFLAGS        =

UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)
CXXFLAGS += -stdlib=libc++
endif

FNORD_IO_TARGET  = libfnord-io.a
FNORD_IO_SOURCES = file.cc fileutil.cc filerepository.cc inputstream.cc outputstream.cc mmappedfile.cc pagemanager.cc
FNORD_IO_OBJECTS = $(subst .cc,.o,$(FNORD_IO_SOURCES))

%.o : %.cc
	$(CXX) -fPIC -c $(CFLAGS) $(CXXFLAGS) -c $<

$(FNORD_IO_TARGET): $(FNORD_IO_OBJECTS)
	ar rcs $(FNORD_IO_TARGET) $(FNORD_IO_OBJECTS)

clean:
	rm -f ${FNORD_IO_OBJECTS} $(FNORD_IO_TARGET)

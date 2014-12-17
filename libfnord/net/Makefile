SHELL          = /bin/sh
CXX            = clang++
CXXFLAGS       = -I../.. -std=c++11
LDFLAGS        =

UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)
CXXFLAGS += -stdlib=libc++
endif

FNORD_NET_TARGET  = libfnord-net.a
FNORD_NET_SOURCES = tcpserver.cc udpserver.cc
FNORD_NET_OBJECTS = $(subst .cc,.o,$(FNORD_NET_SOURCES))

%.o : %.cc
	$(CXX) -fPIC -c $(CFLAGS) $(CXXFLAGS) -c $<

$(FNORD_NET_TARGET): $(FNORD_NET_OBJECTS)
	ar rcs $(FNORD_NET_TARGET) $(FNORD_NET_OBJECTS)

clean:
	rm -f ${FNORD_NET_OBJECTS} $(FNORD_NET_TARGET)

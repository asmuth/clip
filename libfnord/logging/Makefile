SHELL          = /bin/sh
CXX            = clang++
CXXFLAGS       = -I../.. -std=c++11
LDFLAGS        =

UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)
CXXFLAGS += -stdlib=libc++
endif

FNORD_LOGGING_TARGET  = libfnord-logging.a
FNORD_LOGGING_SOURCES = logger.cc logoutputstream.cc loglevel.cc
FNORD_LOGGING_OBJECTS = $(subst .cc,.o,$(FNORD_LOGGING_SOURCES))

%.o : %.cc
	$(CXX) -fPIC -c $(CFLAGS) $(CXXFLAGS) -c $<

$(FNORD_LOGGING_TARGET): $(FNORD_LOGGING_OBJECTS)
	ar rcs $(FNORD_LOGGING_TARGET) $(FNORD_LOGGING_OBJECTS)

clean:
	rm -f ${FNORD_LOGGING_OBJECTS} $(FNORD_LOGGING_TARGET)

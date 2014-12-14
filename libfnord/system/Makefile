SHELL          = /bin/sh
CXX            = clang++
CXXFLAGS       = -I../.. -std=c++11
LDFLAGS        =

FNORD_SYSTEM_TARGET  = libfnord-system.a
FNORD_SYSTEM_SOURCES = signalhandler.cc
FNORD_SYSTEM_OBJECTS = $(subst .cc,.o,$(FNORD_SYSTEM_SOURCES))

%.o : %.cc
	$(CXX) -fPIC -c $(CFLAGS) $(CXXFLAGS) -c $<

$(FNORD_SYSTEM_TARGET): $(FNORD_SYSTEM_OBJECTS)
	ar rcs $(FNORD_SYSTEM_TARGET) $(FNORD_SYSTEM_OBJECTS)

clean:
	rm -f ${FNORD_SYSTEM_OBJECTS} $(FNORD_SYSTEM_TARGET)

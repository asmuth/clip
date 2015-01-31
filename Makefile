all: build test

build:
	mkdir -p build
	(cd build && cmake -GNinja -DCMAKE_BUILD_TYPE=Release .. && ninja)

build-dbg:
	mkdir -p build-dbg
	(cd build-dbg && cmake -GNinja -DCMAKE_BUILD_TYPE=Debug .. && ninja)

test: build
	@find build/tests -name "test-*" -exec ./{} \;

test-dbg: build-dbg
	@find build-dbg/tests -name "test-*" -exec ./{} \;

clean:
	rm -rf build build-dbg

install: build
	(cd build/cmake && make install)

devserver: build-dbg
	mkdir -p /tmp/fnordmetric-data
	./build-dbg/fnordmetric-server --datadir /tmp/fnordmetric-data --verbose

.PHONY: clean build build-dbg test test-dbg clean install devserver

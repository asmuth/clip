all: build test

build:
	mkdir -p build
	(cd build && cmake .. && make)

clean:
	rm -rf build

.PHONY: clean build test

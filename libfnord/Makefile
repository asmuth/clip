all: build test

build:
	mkdir -p build
	(cd build && cmake .. && make)

test: build
	@find build/tests -name "test-*" -exec ./{} \;

clean:
	rm -rf build

.PHONY: clean build test

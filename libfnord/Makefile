all: build

build:
	mkdir -p build
	(cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make)

test: build
	@find build/tests -name "test-*" -exec ./{} \;

clean:
	rm -rf build

.PHONY: clean build test

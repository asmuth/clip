all: build test

build:
	mkdir -p build
	(cd build && cmake -DCMAKE_BUILD_TYPE=Release ../cmake && make)

build-dbg:
	mkdir -p build-dbg
	(cd build-dbg && cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ../cmake && ninja)

test: build
	@find build/tests -name "test-*" -exec ./{} \;

test-dbg: build-dbg
	@find build-dbg/tests -name "test-*" -exec ./{} \;

clean:
	rm -rf build build-dbg .dev_assets

install: build
	(cd build/cmake && make install)

devserver: build-dbg
	mkdir -p /tmp/fnordmetric-data
	rm -rf .dev_assets
	mkdir -p .dev_assets/fnord
	ln -s ../../src/libfnord/fnord-webcomponents/components .dev_assets/fnord/components
	ln -s ../../src/libfnord/fnord-webcomponents/3rdparty .dev_assets/fnord/3rdparty
	ln -s ../../src/libfnord/fnord-webcomponents/themes .dev_assets/fnord/themes
	ln -s ../../src/libfnord/fnord-webcomponents/fnord.js .dev_assets/fnord/fnord.js
	ln -s ../src/fnordmetric/webui .dev_assets/fnordmetric
	DEV_ASSET_PATH=./.dev_assets ./build-dbg/fnordmetric-server --datadir /tmp/fnordmetric-data --verbose

.PHONY: clean build build-dbg test test-dbg clean install devserver

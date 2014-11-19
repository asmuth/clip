FROM ubuntu:14.04
RUN apt-get update \
	&& apt-get install -y curl \
	&& rm -rf /var/lib/apt/lists/*

RUN buildDeps='unzip git gcc make clang++-3.4 cmake libmysqlclient-dev'; \
	set -x \
	&& apt-get update && apt-get install -y $buildDeps --no-install-recommends \
	&& rm -rf /var/lib/apt/lists/* 

RUN mkdir -p /usr/src/ \
	&& cd /usr/src \
	&& curl -R -O https://codeload.github.com/paulasmuth/fnordmetric/zip/master \
	&& unzip master \
	&& cd fnordmetric-master/fnordmetric-core \
	&& make  \
	&& make test \
	&& make install

RUN mkdir /data 
VOLUME /data
WORKDIR /data
EXPOSE 8080 8125
CMD [ "/usr/local/bin/fnordmetric-server --http_port 8080 --statsd_port 8125 --storage_backend disk --datadir /data"]

FROM ubuntu:14.04

RUN groupadd -r redis && useradd -r -g redis redis

RUN apt-get update \
	&& apt-get install -y curl \
	&& rm -rf /var/lib/apt/lists/*

RUN buildDeps='git clang++-3.4 cmake make libmysqlclient-dev'; \
	set -x \
	&& apt-get update && apt-get install -y $buildDeps --no-install-recommends \
	&& rm -rf /var/lib/apt/lists/* \
	&& mkdir -p /usr/src/ \
	&& cd /usr/src \
	&& git clone https://github.com/dolfly/fnordmetric.git \
	&& cd fnordmetric/fnordmetric-core \
	&& make  \
	&& make install \
	&& apt-get purge -y --auto-remove $buildDeps

RUN mkdir /data 
VOLUME /data
WORKDIR /data

#COPY docker-entrypoint.sh /entrypoint.sh
#ENTRYPOINT ["/entrypoint.sh"]

#EXPOSE 8080
#CMD [ "/usr/local/fnordmetric-server" ]

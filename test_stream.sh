#!/bin/sh

if [ "$1" != "stream" ]
 then
 if [ "$2" != "" ]
   then
   sh $0 stream | nc $1 $2
 else
   echo "usage: $0 host port"
   exit
 fi
else
  while true; do
    echo "{\"_type\": \"foobar\"}"
    sleep 0.0001
  done
fi


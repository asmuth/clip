/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "stream.h"
#include "agent.h"
#include "memorybackend.h"

void testStreamKeyGeneration() {
  //fnordmetric::StreamDescription metric_description("mymetric", "kg", "blah");

  fnordmetric::StreamKey<
      fnordmetric::IntegerField,
      fnordmetric::FloatField> stream_key(
      "mymetric",
      fnordmetric::IntegerField("foo"),
      fnordmetric::FloatField("bar"));

  //printf("key=%s\n", stream_key.getKeyString().c_str());
  assert(stream_key.getKeyString() == "83d2f71c457206bf-Ia9f37ed7-F76b77d1a");
}

void testAppendRecord() {
  fnordmetric::Agent agent("myagent", fnordmetric::MemoryBackend::New());

  auto stream = agent.newStream(
      "mystream",
      fnordmetric::IntegerField("count"),
      fnordmetric::FloatField("fnord"));

  stream->appendRecord(42, 23.5);

  auto cursor = stream->getCursor();
  cursor->seekToLast();

}

int main() {
  testStreamKeyGeneration();
  testAppendRecord();
  printf("all tests passed! :)\n");
}

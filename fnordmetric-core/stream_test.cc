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
  bool called = false;

  cursor->seekToLast();
  cursor->getRow([&called] (const uint8_t* data, size_t len, uint64_t time) {
    fnordmetric::RecordReader reader(data, len);
    assert(len == 18);
    assert(fnordmetric::WallClock::getUnixMillis() - time < 10);
    int64_t count;
    double fnord;
    assert(reader.readInteger(&count));
    assert(reader.readFloat(&fnord));
    assert(count == 42);
    assert(fnord == 23.5);
    called = true;
  });

  assert(called);
  assert(cursor->next() == false);
  stream->appendRecord(42, 23.5);
  assert(cursor->next() == true);
}

int main() {
  testStreamKeyGeneration();
  testAppendRecord();
  printf("all tests passed! :)\n");
}

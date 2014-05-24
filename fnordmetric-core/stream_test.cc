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
#include "filebackend.h"

void testStreamKeyGeneration() {
  //fnordmetric::StreamDescription metric_description("mymetric", "kg", "blah");

  fnordmetric::StreamKey<
      fnordmetric::IntegerField,
      fnordmetric::IntegerField> stream_key(
      "mymetric",
      fnordmetric::IntegerField("foo"),
      fnordmetric::IntegerField("bar"));

  printf("key: %s\n", stream_key.getKeyString().c_str());

  assert(stream_key.getKeyString() == "83d2f71c457206bf-Ia9f37ed7-I76b77d1a");
}

void testAppendRecord() {
  fnordmetric::Agent agent("my_fnordmetric_agent",
      fnordmetric::FileBackend::openFile("/tmp/fm-test.db"));

  auto stream = agent.newStream(
      "mystream",
      fnordmetric::IntegerField("count"),
      fnordmetric::IntegerField("fnord"));

  stream->appendRecord(123, 123.4f);
}

int main() {
  testStreamKeyGeneration();
  testAppendRecord();
  printf("all tests passed! :)\n");
}

// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

class SampleInstruction(key: String, mode: String, flush_interval: String, value: String) extends AbstractInstruction {

  def execute : String = {
    val bucket = BucketFactory.find_or_create_bucket(
      BucketKey(key, mode, flush_interval))

    bucket.sample_and_flush(java.lang.Double.parseDouble(value))

    "OK"
  }

}

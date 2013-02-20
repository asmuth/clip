// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

case class MetricKey(key: String, mode: String, flush_interval: Long)

class Metric(key: MetricKey) {
  val bucket = BucketFactory.new_bucket(key.mode)
  var rbuf = new RingBuffer[Double](10)

  def sample(value: Double) = this.synchronized {
    bucket.flush_every(key.flush_interval, (
      (time, value) => flush(time, value) ))

    bucket.sample(value)
  }

  def flush(time: Long, value: Double) = this.synchronized {
    rbuf.push(value)
    println("RINGBUF", rbuf.tail(10))
  }

}

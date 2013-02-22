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
  var rbuf = new RingBuffer[(Long, Double)](1000)
  var rbuf_seek_pos = 0

  // adds a value to this metric
  def sample(value: Double) = {
    this.synchronized {
      bucket.flush_every(key.flush_interval, (
        (time, value) => flush_bucket(time, value) ))
    }

    bucket.sample(value)
  }

  // adds an aggregated value to the in memory ring buffer after it has been
  // flushed from the bucket
  private def flush_bucket(time: Long, value: Double) = {
    if (rbuf.remaining == 0) {

      if (rbuf_seek_pos < 1)
        flush_rbuf

      if (rbuf_seek_pos < 1)
        throw new Exception("flush_rbuf failed")

      rbuf.seek(1)
      rbuf_seek_pos -= 1
    }

    rbuf.push(((time, value)))
    //println("RINGBUF", rbuf.tail(10))
  }

  // tries to persist as much data from the in memory ring buffer to disk
  // as possible but doesnt remove it from the buffer yet
  def flush_rbuf = this.synchronized {
    val flush_range = rbuf.size - rbuf_seek_pos
    //println("RBUF_FLUSH", rbuf.tail(flush_range))
    rbuf_seek_pos += flush_range
  }

}

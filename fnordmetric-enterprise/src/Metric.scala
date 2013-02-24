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
  val swap = new SwapFile(key)
  var rbuf = new RingBuffer[(Long, Double)](1000)
  var rbuf_seek_pos = 0

  // adds a value to the metric's bucket and tries to flush the bucket
  def sample(value: Double) = this.synchronized {

    bucket.flush_every(key.flush_interval, (
      (time, value) => flush_bucket(time, value) ))

    bucket.sample(value)
  }

  // adds an aggregated value to the in memory ring buffer after it has
  // been flushed from the bucket
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

    flush_rbuf // FIXPAUL: remove me
  }

  // tries to persist as much data from the in memory ring buffer to disk
  // as possible but doesnt remove it from the buffer yet
  def flush_rbuf = this.synchronized {
    val flush_range = rbuf.size - rbuf_seek_pos

    for (sample <- rbuf.tail(flush_range))
      swap.put(sample._1, sample._2)

    swap.flush
    rbuf_seek_pos += flush_range
  }


  // returns this metrics value at time0 if a value was recorded at that
  // point in time
  def value_at(time0: Long) : Option[Double] = {
    val values = values_in(time0, 0)

    if (values.size > 0)
      Some(values.first._2)
    else
      None
  }

  // returns all aggregated values for this metric in the specified time
  // range. if time1 is 0 then only the first value at time0 is returned
  def values_in(time0: Long, time1: Long) : List[(Long, Double)] = {
    val lst = List[(Long, Double)]()

    // here be dragons

    lst
  }

}

// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

import java.io.RandomAccessFile
import java.io.File
import java.nio.ByteBuffer
import java.nio.ByteOrder

class SwapFile(metric_key: MetricKey) {

  val file_name = "metric-" + metric_key.key +
    metric_key.mode + "-" + metric_key.flush_interval

  val file = new RandomAccessFile(new File(
    FnordMetric.CONFIG('swap_prefix), file_name), "rwd")

  val buffer = ByteBuffer.allocate(512)
  buffer.order(ByteOrder.BIG_ENDIAN)

  def put(time: Long, value: Double) : Unit = {
    val bvalue = java.lang.Double.doubleToLongBits(value)

    if (buffer.remaining < 18)
      flush

    buffer.putShort(0x1717)
    buffer.putLong(time)
    buffer.putLong(bvalue)
  }

  def flush : Unit = {
    file.seek(file.length)
    file.write(buffer.array)
    buffer.rewind
  }

}


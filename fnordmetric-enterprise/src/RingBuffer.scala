// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

class RingBuffer[T: Manifest] {

  private var capacity : Int = 20
  var start    : Int = 0
  var end      : Int = 0

  var backend = new Array[T](capacity)

  // appends a new item to the ringbuffer, if the ring buffer is full,
  // the oldest item is pushed out
  def push(item: T) : Unit = {
    backend.synchronized {
      var nxt = (end + 1) % capacity

      end = nxt

      if (end == start)
        start += 1
    }

    backend(end) = item
  }

  // retrieves items from the list buffer by walking the list buffer in
  // reverse chronological order (from most recent to oldest item) and
  // applying a lambda function to each item. all items until the first
  // item for which the lambda function returns false are returned.
  def retrieve(exit: (T => Boolean)) : List[T] =
    null


}

// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

class RingBuffer[T: Manifest](capacity: Int) {
  private val backend  = new Array[T](capacity)
  private var size     : Int = 0
  private var position : Int = 0

  // appends a new item to the ringbuffer, if the ring buffer is full,
  // the oldest item is pushed out
  def push(item: T) : Unit = {
    backend.synchronized {
      position = (position + 1) % capacity

      if (size < capacity)
        size += 1

      backend(position) = item
    }
  }

  // retrieves the last max items from the list buffer by walking the list
  // buffer in reverse chronological order (from most recent to oldest)
  def retrieve(max: Int = 0) : List[T] = {
    val cap = scala.math.min(size,
      if (max == 0) capacity else max)

    val lst = new Array[T](cap)

    backend.synchronized {
      (0 until cap).foreach {
        n => lst(n) = backend(index_at(n))
      }
    }

    lst.toList
  }

  private def index_at(ind: Int) : Int =
    (((position - ind) % capacity) + capacity) % capacity

}

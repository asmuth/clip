// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

class RingBuffer[T: Manifest](capacity: Int) {
  private val backend = new Array[T](capacity)

  private var end   : Int = -1
  private var start : Int = 0

  // the numer of elements that this ring buffer currently contains
  var size  : Int = 0

  // appends a new item. the remaining number of free slots must be checked
  // before appending
  def push(item: T) : Unit = {
    if (size == capacity)
      throw new Exception("ring buffer is full")

    size += 1

    end = (end + 1) % capacity
    backend(end) = item
  }

  // retrieves the first max items from the ring buffer by walking the ring
  // buffer in chronological order (from oldest to most recent)
  def head(max: Int) : List[T] = {
    val lst = new Array[T](scala.math.min(size, max))

    for (ind <- (0 until lst.size))
      lst(ind) = backend((start + ind) % capacity)

    lst.toList
  }

  // retrieves the last max items from the ring buffer by walking the ring
  // buffer in reverse chronological order (from most recent to oldest)
  def tail(max: Int) : List[T] = {
    val lst = new Array[T](scala.math.min(size, max))

    for (ind <- (0 until lst.size))
      lst(ind) = backend((
        ((end - ind) % capacity) + capacity) % capacity)

    lst.toList
  }

  // removes the first num items from the start of the ring buffer (oldest
  // items get removed first)
  def seek(num: Int) = {
    start = (start + num) % capacity
    size -= num
  }

  // returns the remaning number of free slots in the ringbuffer
  def remaining : Int =
    capacity - size

}

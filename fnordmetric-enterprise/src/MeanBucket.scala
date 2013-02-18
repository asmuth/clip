
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

class MeanBucket(_key: BucketKey) extends AbstractBucket {
  val key = _key
  var tmp_sum : Double = 0
  var tmp_cnt : Int = 0

  def sample(value: Double) : Unit = {
    tmp_sum += value
    tmp_cnt += 1
  }

  def flush : Double = {
    val res = if (tmp_cnt == 0)
      0
    else
      tmp_sum / tmp_cnt

    tmp_sum = 0.toDouble
    tmp_cnt = 0

    println("flush: " + res)
    res
  }

}

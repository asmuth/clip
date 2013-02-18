// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

import java.util.concurrent.ConcurrentHashMap

object BucketFactory {

  val buckets = new ConcurrentHashMap[BucketKey, AbstractBucket]()

  def find_or_create_bucket(key: BucketKey) : AbstractBucket = {
    var bucket : AbstractBucket = buckets.get(key)

    if (bucket == null) {
      buckets.synchronized {
        bucket = buckets.get(key)

        if (bucket == null) {
          bucket = create_bucket(key)
          buckets.put(key, bucket)
        }
      }
    }

    bucket
  }

  def create_bucket(key: BucketKey) : AbstractBucket = {
    new SumBucket
  }

}

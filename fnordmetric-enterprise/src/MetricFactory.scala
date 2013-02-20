
// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

import scala.collection.mutable.HashMap

object MetricFactory {

  val metric_map = new HashMap[MetricKey, Metric]()

  def get_metric(key: MetricKey) : Metric = {
    var metric : Metric = metric_map.getOrElse(key, null)

    if (metric == null) {
      this.synchronized {
        metric = metric_map.getOrElse(key, null)

        if (metric == null) {
          metric = new Metric(key)
          metric_map += ((key, metric))
        }
      }
    }

    metric
  }

}

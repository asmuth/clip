// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

class ValueAtInstruction(key: MetricKey, time: Long) extends AbstractInstruction {

  def execute : String = {
    val metric = MetricFactory.get_metric(key)
    val value = metric.value_at(time * 1000).getOrElse(null)

    if (value == null)
      "null"
    else
      FnordMetric.number_format.format(value)
  }

}

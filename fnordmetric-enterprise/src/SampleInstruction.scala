// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

class SampleInstruction(key: MetricKey, value: Double) extends AbstractInstruction {

  def execute : String = {
    MetricFactory.get_metric(key).sample(value)
    "OK"
  }

}

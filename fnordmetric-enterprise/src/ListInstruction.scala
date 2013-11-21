// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

class ListInstruction extends AbstractInstruction {

  def execute : String = {
    val out = new StringBuffer()

    for (key <- MetricFactory.metric_map.keys) {
      out.append(key.key)
      out.append(key.mode)
      out.append("-")
      out.append(key.flush_interval / 1000)
      out.append(" ")
    }

    if (out.length < 1) {
      out.append("null ")
    }

    out.setLength(out.length - 1)
    out.toString
  }

}

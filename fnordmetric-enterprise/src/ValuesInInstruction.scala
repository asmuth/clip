// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

class ValuesInInstruction(key: MetricKey, time0: Long, time1: Long) extends AbstractInstruction {

  def execute : String = {
    val metric = MetricFactory.get_metric(key)
    val values = metric.values_in(time1 * 1000, time0 * 1000)

    // we estimate that every tuple will need around 25 byte in its
    // ascii representation
    val resp = new StringBuffer(values.size * 25)

    if (values.size == 0)
      return "null"

    values.foreach { cur =>
      resp.append(cur._1)
      resp.append(":")

      resp.append(FnordMetric.number_format.format(cur._2))

      // if this is the last item, we write an extra whitespace
      // character as checking for the last item would be expensive
      resp.append(" ")
    }

    // we have to remove the last whitespace as the if statement
    // in the loop would be to expensive
    resp.setLength(resp.length - 1)

    resp.toString
  }

}

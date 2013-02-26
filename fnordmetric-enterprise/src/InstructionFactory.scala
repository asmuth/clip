// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

object InstructionFactory {

  val X_METRICKEY = """(.*)(mean|sum)-([0-9]+)"""
  val X_SAMPLE    = ("""^SAMPLE """ + X_METRICKEY + """ ([0-9]+\.?[0-9]*)$""").r
  val X_VALUESIN  = ("""^VALUES_?IN """ + X_METRICKEY + """ ([0-9]+) ([0-9]+)$""").r
  val X_VALUEAT   = ("""^VALUE_?AT """ + X_METRICKEY + """ ([0-9]+)$""").r

  def parse(str: String) : AbstractInstruction = str match {

    case X_SAMPLE(key, mode, flush_interval, value) =>
      new SampleInstruction(MetricKey(key, mode,
        java.lang.Double.parseDouble(flush_interval).longValue * 1000),
        java.lang.Double.parseDouble(value))

    case X_VALUESIN(key, mode, flush_interval, time0, time1) =>
      new ValuesInInstruction(MetricKey(key, mode,
      java.lang.Double.parseDouble(flush_interval).longValue * 1000),
      java.lang.Long.parseLong(time0), java.lang.Long.parseLong(time1))

    case X_VALUEAT(key, mode, flush_interval, time) =>
      new ValueAtInstruction(MetricKey(key, mode,
      java.lang.Double.parseDouble(flush_interval).longValue * 1000),
      java.lang.Long.parseLong(time))

    case _ =>
      new ErrorInstruction("invalid command")

  }

}

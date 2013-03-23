// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

object InstructionFactory {

  val X_METRICKEY = """(.*)(mean|sum)-([0-9]+)"""
  val X_TIMESPEC  = """(-?[0-9\.]*[acedhioumnrstyw]*)"""
  val X_TUNIX     = """([0-9]+)""".r
  val X_TSECS     = """-([0-9]+(?:\.[0-9]+)?)s?(?:ec(?:ond)?(?:s?))?""".r
  val X_TMINS     = """-([0-9]+(?:\.[0-9]+)?)m(?:in(?:ute)?(?:s?))?""".r
  val X_THOUR     = """-([0-9]+(?:\.[0-9]+)?)h(?:our(?:s?))?""".r
  val X_TDAYS     = """-([0-9]+(?:\.[0-9]+)?)d(?:ay(?:s?))?""".r
  val X_SAMPLE    = ("""^SAMPLE """ + X_METRICKEY + """ ([0-9]+\.?[0-9]*)$""").r
  val X_VALUESIN  = ("""^VALUES_?IN """ + X_METRICKEY + " " + X_TIMESPEC + " " + X_TIMESPEC).r
  val X_VALUEAT   = ("""^VALUE_?AT """ + X_METRICKEY + " " + X_TIMESPEC).r

  def parse(str: String) : AbstractInstruction = str match {

    case X_SAMPLE(key, mode, flush_interval, value) =>
      new SampleInstruction(MetricKey(key, mode,
        java.lang.Double.parseDouble(flush_interval).longValue * 1000),
        java.lang.Double.parseDouble(value))

    case X_VALUESIN(key, mode, flush_interval, time0, time1) =>
      new ValuesInInstruction(MetricKey(key, mode,
      java.lang.Double.parseDouble(flush_interval).longValue * 1000),
      parseTime(time0), parseTime(time1))

    case X_VALUEAT(key, mode, flush_interval, time) => {
      new ValueAtInstruction(MetricKey(key, mode,
      java.lang.Double.parseDouble(flush_interval).longValue * 1000),
      parseTime(time))
    }

    case "LIST" =>
      new ListInstruction()

    case _ =>
      new ErrorInstruction("invalid command")

  }

  private def parseTime(str: String) : Long = str match {

    case "now" =>
      FnordMetric.now

    case X_TUNIX(numval) =>
      java.lang.Long.parseLong(numval) * 1000

    case X_TSECS(numval) =>
      FnordMetric.now - (java.lang.Long.parseLong(numval) * 1000)

    case X_TMINS(numval) =>
      FnordMetric.now - (java.lang.Long.parseLong(numval) * 60000)

    case X_THOUR(numval) =>
      FnordMetric.now - (java.lang.Long.parseLong(numval) * 3600000)

    case X_TDAYS(numval) =>
      FnordMetric.now - (java.lang.Long.parseLong(numval) * 86400000)

  }

}

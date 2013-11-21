// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

import java.io.FileWriter
import java.io.PrintWriter

object AOFile {

  lazy val writer : Option[PrintWriter] = get_writer

  def append(key: MetricKey, time: Long, value: Double) = {
    writer map { w =>
      val out = new StringBuffer()
      out.append(time)
      out.append(" ")
      out.append(key.key)
      out.append(key.mode)
      out.append("-")
      out.append(key.flush_interval / 1000)
      out.append(" ")
      out.append(value)

      synchronized {
        w.println(out.toString)
        w.flush()
      }
    }
  }

  private def get_writer() : Option[PrintWriter] = {
    if (FnordMetric.CONFIG contains 'aofile) {
      val file_writer = new FileWriter(
        FnordMetric.CONFIG('aofile), true)

      return Some(new PrintWriter(file_writer))
    } else {
      return None
    }
  }

}

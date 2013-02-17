// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

object InstructionFactory {

  val X_SAMPLE = """^SAMPLE (.*)(delta|mean|sum)-([0-9]+) ([0-9]+\.?[0-9]+)$""".r

  def parse(str: String) : AbstractInstruction = str match {

    case X_SAMPLE(key, mode, flush_interval, value) =>
      new SampleInstruction(key, mode, flush_interval, value)

    case _ =>
      new ErrorInstruction("invalid command")

  }

}

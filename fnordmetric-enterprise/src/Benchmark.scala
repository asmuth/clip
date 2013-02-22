// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

object Benchmark {

  def run : Unit = {
    print_title("Metric#sample")
    for (n <- List(200000, 1000000, 5000000))
      for (t <- List(1, 2, 4, 8, 16, 32, 64))
      bm_metric(n, t)

  }

  private def bm_metric(samples: Int, threads: Int) : Unit = {
    val preheat = 10
    val tests = 100

    val tdiff = (0.toLong /: (1 to tests + preheat)) ((s, n) => {
      val metric = new Metric(
        MetricKey("fnord", "sum", 1.toLong))

      val t0 = FnordMetric.now

      for (n <- (1 to samples))
        metric.sample(23)

      if (n < preheat)
        0
      else
        s + (FnordMetric.now - t0)
    })

    print_res(samples + " values, " + threads + " thread(s)",
      tdiff / tests)
  }

  // HACK !!! ;)
  private def print_res(title: String, tdiff: Long) =
    println("  * " + title +
      (("" /: (1 to (30 - title.length)))((m,c) => m + " ")) + " => " +
      tdiff + "ms")

  // HACK !!! ;)
  private def print_title(title: String) =
    println(title + "\n" +
      (("" /: (1 to (title.length)))((m,c) => m + "=")) + "\n")

}

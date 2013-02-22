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
      for (t <- List(1, 4, 16))
        bm_metric(n, t)

  }

  private def bm_metric(samples: Int, threads: Int) : Unit =
    print_res(samples + " values, " + threads + " thread(s)",
      mean_with_preheat(50, 10, (() => {

        val metric = new Metric(
          MetricKey("fnord", "sum", 1.toLong))

        measure(() => {
          in_parallel(threads, (() => {

            for (n <- (1 to (samples / threads)))
              metric.sample(23)

          }))
        })
      })))


  private def measure(proc: => Function0[Unit]) : Long = {
    val tstart = FnordMetric.now
    proc()
    FnordMetric.now - tstart
  }

  private def mean_with_preheat(tests: Int, preheat: Int, proc:  Function0[Long]) : Long =
    ((0.toLong /: (1 to tests + preheat)) ((s, n) => {
      val v = proc()
      if (n < preheat) 0 else s + v
    }) / (tests + preheat))


  private def in_parallel(threads: Int, proc: Function0[Unit]) : Unit =
    ((1 to threads) map (n => {
      val t = new Thread { override def run = { proc() }}
      t.start; t
    })) map { t => t.join }


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

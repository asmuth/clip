// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

object Benchmark {

  def run : Unit = {
    FnordMetric.DEFAULTS.foreach(d =>
      FnordMetric.CONFIG += d)

    print_title("MetricFactory#get_metric")
    for (t <- List(1, 16, 64))
      bm_metric_factory(1000, t)

    print_title("Metric#values_in")
    for (n <- List(10L, 60L, 180L, 3600L, 84600L))
      for (t <- List(1, 4, 16, 64))
        bm_metric_values_in(n, t)

    print_title("Metric#value_at")
    for (n <- List(10L, 60L, 180L, 3600L, 84600L))
      for (t <- List(1, 4, 16, 64))
        bm_metric_value_at(n, t)


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


  private def bm_metric_values_in(range: Long, threads: Int) : Unit =
    print_res(range + " seconds, " + threads + " thread(s)",
      mean_with_preheat(50, 10, (() => {

        val metric = new Metric(
          MetricKey("fnord", "sum", 1.toLong))

        measure(() => {
          in_parallel(threads, (() => {

            for (n <- (1 to 100))
              metric.values_in(metric.bucket.next_flush, metric.bucket.next_flush - range)

          }))
        }) / 100
      })))


  private def bm_metric_value_at(ago: Long, threads: Int) : Unit =
    print_res(ago + " seconds, " + threads + " thread(s)",
      mean_with_preheat(50, 10, (() => {

        val metric = new Metric(
          MetricKey("fnord", "sum", 1.toLong))

        measure(() => {
          in_parallel(threads, (() => {

            for (n <- (1 to 100))
              metric.value_at(metric.bucket.next_flush - ago)

          }))
        }) / 100
      })))


  private def bm_metric_factory(metrics: Int, threads: Int) : Unit =
    print_res(metrics + " metrics, " + threads + " thread(s)",
      mean_with_preheat(30, 10, (() => {
        measure(() => {
          in_parallel(threads, (() => {
            for (x <- (0 to 100))
              MetricFactory.get_metric(MetricKey(
                "benchmark-metric" + (scala.math.random * metrics).toInt,
                "sum", 1.toLong))
          }))
        }) / 100
      })))


  private def measure(proc: => Function0[Unit]) : Long = {
    val tstart = System.nanoTime
    proc()
    System.nanoTime - tstart
  }

  private def mean_with_preheat(tests: Int, preheat: Int, proc:  Function0[Long]) : Long =
    ((0.toLong /: (1 to tests + preheat)) ((s, n) => {
      val v = proc()
      if (n < preheat) 0 else s + v
    }) / tests)


  private def in_parallel(threads: Int, proc: Function0[Unit]) : Unit =
    ((1 to threads) map (n => {
      val t = new Thread { override def run = { proc() }}
      t.start; t
    })) map { t => t.join }


  // HACK !!! ;)
  private def print_res(title: String, tdiff: Long) =
    println("  * " + title +
      (("" /: (1 to (30 - title.length)))((m,c) => m + " ")) + " => " +
      (tdiff/1000000.0) + "ms")

  // HACK !!! ;)
  private def print_title(title: String) =
    println("\n\n" + title + "\n" +
      (("" /: (1 to (title.length)))((m,c) => m + "=")) + "\n")

}

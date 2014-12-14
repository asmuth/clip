/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_TEST_BENCHMARK_H
#define _FNORD_TEST_BENCHMARK_H
#include <stdlib.h>
#include <stdint.h>
#include "fnord/base/datetime.h"

namespace fnord {

class Benchmark {
public:

  class BenchmarkResult {
  public:
    BenchmarkResult(uint64_t total_time_nanos, uint64_t num_iterations);

    /**
     * Returns the mean time per run over all runs in nanoseconds
     */
    uint64_t meanRuntimeNanos() const;

    /**
     * Returns the number of runs/operations per second
     */
    double ratePerSecond() const;

    /**
     * How many times did we run the benchmark?
     */
    uint64_t numIterations() const;

  protected:
    const uint64_t total_time_nanos_;
    const uint64_t num_iterations_;
  };

  /**
   * Execute the provided function N times and return the measured timing data
   *
   * @param subject the function to execute
   * @param num_iterations how often to execute the benchmark
   */
  static BenchmarkResult benchmark(
      std::function<void()> subject,
      uint64_t num_iterations = 1000);

  /**
   * Execute the provided function N * M times and print the measured timing
   * data as a table
   *
   * @param subject the function to execute
   * @param num_iterations how often to execute the benchmark per round
   * @param num_rounds how many rounds of benchmark to execute
   */
  static void benchmarkAndPrint(
      std::function<void()> subject,
      uint64_t num_iterations = 1000,
      uint64_t num_rounds = 10);

  /**
   * Print a benchmark result as a table
   */
  static void printResultTable(
    const std::string& label,
    const BenchmarkResult& result,
    bool append = false);

};

}
#endif

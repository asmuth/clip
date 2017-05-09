/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <metrictools/config_list.h>
#include <metrictools/timeseries.h>
#include <metrictools/storage/op.h>

namespace fnordmetric {

/**
 * The fetch storage operation loads data for one or more metrics from the data
 * store
 */
class FetchStorageOp : public StorageOp {
public:

  /**
   * A fetch request
   */
  struct FetchRequest {

    /**
     * A snapshot of the metric config for the metric for which the data should
     * be fetched. The snapshot must be consistend with all filters and other
     * settings in the request
     */
    std::shared_ptr<const MetricConfig> metric;

    /**
     * If true, fetch the latest value for each metric label
     */
    bool fetch_last;

    /**
     * If true, fetch historical data for each metric label
     */
    bool fetch_history;

    /**
     * The begin of the time window for historical data (if requested) as number
     * of microseconds since epoch (inclusive)
     */
    uint64_t history_time_begin;

    /**
     * The limit of the time window for historical data (if requested) as number
     * of microseconds since epoch (exclusive)
     */
    uint64_t history_time_limit;

  };

  /**
   * Each fetch response contains data for exactly one metric label
   */
  struct FetchResponse {

    /**
     * The original request for which this response was generated
     */
    const FetchRequest* request;

    /**
     * The label path of this response
     */
    MetricLabels label;

    /**
     * If the latest value was requested, the latest value for this label
     */
    std::string last_value;

    /**
     * If historical data was requested, the historical data for this label
     */
    TimeseriesRef history;

  };

  /**
   * Create a new insert operation
   *
   * @param global_config A snapshot of the global config.
   */
  FetchStorageOp(std::shared_ptr<const GlobalConfig> global_config) noexcept;

  /**
   * Add a request/subquery to the fetch operation
   */
  void addRequest(FetchRequest&& request) noexcept;

  /**
   * Get the list of requests
   */
  const std::vector<FetchRequest>& getRequests() const noexcept;

  /**
   * Add a response to the fetch operation
   */
  void addResponse(FetchResponse&& response) noexcept;

  /**
   * Get the list of requests
   */
  const std::vector<FetchResponse>& getResponses() const noexcept;

  /**
   * Get the global config snapshot
   */
  std::shared_ptr<const GlobalConfig> getGlobalConfig() const noexcept;

protected:
  std::shared_ptr<const GlobalConfig> global_config_;
  std::vector<FetchRequest> requests_;
  std::vector<FetchResponse> responses_;
};

/**
 * Print a debug dump of the operation
 */
std::ostream& operator<<(std::ostream& out, const FetchStorageOp& op);

} // namespace fnordmetric


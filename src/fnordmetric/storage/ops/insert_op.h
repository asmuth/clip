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
#include <fnordmetric/config_list.h>
#include <fnordmetric/storage/op.h>

namespace fnordmetric {

/**
 * The insert storage operation appends one or more new measurements to the
 * data store
 */
class InsertStorageOp : public StorageOp {
public:

  /**
   * Create a new insert operation
   *
   * @param global_config A snapshot of the global config. The snapshot must
   * be consitent with the instance_path_global values in all measurements in
   * the operation.
   */
  InsertStorageOp(std::shared_ptr<const GlobalConfig> global_config) noexcept;

  /**
   * A measurement to be inserted
   */
  struct Measurement {

    /**
     * The time at which this measurement was recorded as number of microseconds
     * since epoch
     */
    uint64_t time;

    /**
     * A snapshot of the metric config for the metric into which the measurement
     * should be inserted. The snapshot of the config should be created at the
     * time of capture and must be consistent with the values in
     * instance_path_metric.
     */
    std::shared_ptr<const MetricConfig> metric;

    /**
     * The metric instance path. The vector must have a length equal to the
     * number of elements in the global instance path config plus the number of
     * elements in the metric-specific instance path config. Each position in
     * the vector corresponds to the respective position in the path definition.
     */
    MetricInstancePath instance;

    /**
     * The value to insert encoded as a human-readable string. For numeric
     * metric types, the value must contain a valid string representation of
     * a floating point number (e.g. the string "12.3" is valid, while "blah"
     * is not)
     */
    std::string value;

  };

  /**
   * Add a masurement to be inserted to the operation
   */
  void addMeasurement(Measurement&& measurement) noexcept;

  /**
   * Add a measurement but supply the instance path as a dictionary of key=value
   * pairs. The method will construct a measurement struct (which needs the
   * instance path as a dense vector of strings in the correct order) and then
   * add it to the operation.
   *
   * Currently, this method will silently ignore keys that are not required to
   * construct the instance path. It will also default all missing keys with
   * an empty string. However, this behaviour might change so the return code
   * should be checked for future-proofness.
   **/
  ReturnCode addMeasurement(
      std::shared_ptr<const MetricConfig> metric,
      const std::map<std::string, std::string>& instance_path,
      const std::string& value) noexcept;

  /**
   * Get the list of measurements to be inserted
   */
  const std::vector<Measurement>& getMeasurements() const noexcept;

  /**
   * Get the global config snapshot
   */
  std::shared_ptr<const GlobalConfig> getGlobalConfig() const noexcept;

protected:
  std::shared_ptr<const GlobalConfig> global_config_;
  std::vector<Measurement> measurements_;
};

} // namespace fnordmetric


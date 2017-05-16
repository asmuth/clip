/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/timeseries.h>

namespace fnordmetric {

template <>
std::string Timeseries<std::string>::getTypeName() const {
  return "string";
}

template <>
std::string Timeseries<double>::getTypeName() const {
  return "float64";
}

template <>
std::string Timeseries<int64_t>::getTypeName() const {
  return "int64";
}

template <>
std::string Timeseries<uint64_t>::getTypeName() const {
  return "uint64";
}

template<>
ReturnCode convertTimeseries(
    const Timeseries<double>& in,
    Timeseries<std::string>* out) {
  for (size_t i = 0; i < in.size(); ++i) {
    out->timestamps.emplace_back(in.timestamps[i]);
    out->values.emplace_back(std::to_string(in.values[i]));
  }

  return ReturnCode::success();
}

template<>
ReturnCode convertTimeseries(
    const Timeseries<uint64_t>& in,
    Timeseries<std::string>* out) {
  for (size_t i = 0; i < in.size(); ++i) {
    out->timestamps.emplace_back(in.timestamps[i]);
    out->values.emplace_back(std::to_string(in.values[i]));
  }

  return ReturnCode::success();
}

template<>
ReturnCode convertTimeseries(
    const Timeseries<int64_t>& in,
    Timeseries<std::string>* out) {
  for (size_t i = 0; i < in.size(); ++i) {
    out->timestamps.emplace_back(in.timestamps[i]);
    out->values.emplace_back(std::to_string(in.values[i]));
  }

  return ReturnCode::success();
}

template<>
ReturnCode convertTimeseries(
    const Timeseries<std::string>& in,
    Timeseries<double>* out) try {
  for (size_t i = 0; i < in.size(); ++i) {
    out->timestamps.emplace_back(in.timestamps[i]);
    out->values.emplace_back(std::stod(in.values[i]));
  }

  return ReturnCode::success();
} catch (...) {
  return ReturnCode::error("ERUNTIME", "error while converting timeseries to double");
}

template<>
ReturnCode convertTimeseries(
    const Timeseries<uint64_t>& in,
    Timeseries<double>* out) {
  for (size_t i = 0; i < in.size(); ++i) {
    out->timestamps.emplace_back(in.timestamps[i]);
    out->values.emplace_back(in.values[i]);
  }

  return ReturnCode::success();
}

template<>
ReturnCode convertTimeseries(
    const Timeseries<int64_t>& in,
    Timeseries<double>* out) {
  for (size_t i = 0; i < in.size(); ++i) {
    out->timestamps.emplace_back(in.timestamps[i]);
    out->values.emplace_back(in.values[i]);
  }

  return ReturnCode::success();
}

template<>
ReturnCode convertTimeseries(
    const Timeseries<std::string>& in,
    Timeseries<uint64_t>* out) try {
  for (size_t i = 0; i < in.size(); ++i) {
    out->timestamps.emplace_back(in.timestamps[i]);
    out->values.emplace_back(std::stoull(in.values[i]));
  }

  return ReturnCode::success();
} catch (...) {
  return ReturnCode::error("ERUNTIME", "error while converting timeseries to uint64");
}

template<>
ReturnCode convertTimeseries(
    const Timeseries<double>& in,
    Timeseries<uint64_t>* out) {
  for (size_t i = 0; i < in.size(); ++i) {
    out->timestamps.emplace_back(in.timestamps[i]);
    out->values.emplace_back(in.values[i]);
  }

  return ReturnCode::success();
}

template<>
ReturnCode convertTimeseries(
    const Timeseries<int64_t>& in,
    Timeseries<uint64_t>* out) {
  for (size_t i = 0; i < in.size(); ++i) {
    out->timestamps.emplace_back(in.timestamps[i]);
    out->values.emplace_back(in.values[i]);
  }

  return ReturnCode::success();
}

template<>
ReturnCode convertTimeseries(
    const Timeseries<std::string>& in,
    Timeseries<int64_t>* out) try {
  for (size_t i = 0; i < in.size(); ++i) {
    out->timestamps.emplace_back(in.timestamps[i]);
    out->values.emplace_back(std::stoull(in.values[i]));
  }

  return ReturnCode::success();
} catch (...) {
  return ReturnCode::error("ERUNTIME", "error while converting timeseries to int64");
}

template<>
ReturnCode convertTimeseries(
    const Timeseries<double>& in,
    Timeseries<int64_t>* out) {
  for (size_t i = 0; i < in.size(); ++i) {
    out->timestamps.emplace_back(in.timestamps[i]);
    out->values.emplace_back(in.values[i]);
  }

  return ReturnCode::success();
}

template<>
ReturnCode convertTimeseries(
    const Timeseries<uint64_t>& in,
    Timeseries<int64_t>* out) {
  for (size_t i = 0; i < in.size(); ++i) {
    out->timestamps.emplace_back(in.timestamps[i]);
    out->values.emplace_back(in.values[i]);
  }

  return ReturnCode::success();
}

void timeseriesRate(Timeseries<double>* ts, uint64_t rate) {
  switch (ts->size()) {
    case 0:
      return;
    case 1:
      ts->clear();
      return;
  }

  uint64_t ct = ts->timestamps.front();
  double cv = ts->values.front();

  for (size_t i = 1; i < ts->size(); ++i) {
    auto v = ts->values[i];
    if (v > cv) {
      v -= cv;
    }

    ts->values[i - 1] = (v / (ts->timestamps[i] - ct)) * rate;
    ts->timestamps[i - 1] = ts->timestamps[i];

    ct = ts->timestamps[i];
    cv = ts->values[i];
  }

  ts->timestamps.pop_back();
  ts->values.pop_back();
}

} // namespace fnordmetric


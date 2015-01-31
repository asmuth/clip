/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-base/ieee754.h>
#include <fnord-metricdb/backends/disk/tokenindex.h>
#include <fnord-metricdb/backends/disk/samplereader.h>

namespace fnord {
namespace metric_service {
namespace disk_backend {

AbstractSampleReader::AbstractSampleReader(
    void* data,
    size_t size,
    TokenIndex* token_index) :
    fnord::util::BinaryMessageReader(data, size),
    token_index_(token_index),
    labels_read_(false) {}

const std::vector<std::pair<std::string, std::string>>&
    AbstractSampleReader::labels() {
  if (!labels_read_) {
    seekTo(label_offset_);
    labels_read_ = true;

    while (pos_ < size_) {
      auto key = readToken();
      auto value = readToken();
      labels_.emplace_back(key, value);
    }
  }

  return labels_;
}

std::vector<std::pair<uint32_t, std::string>>
    AbstractSampleReader::tokenDefinitions() {
  seekTo(label_offset_);

  std::vector<std::pair<uint32_t, std::string>> token_definitions;
  while (pos_ < size_) {
    auto token_ref = *readUInt32();
    uint32_t string_len;
    uint32_t token_def = 0;

    if (token_ref == 0xffffffff) {
      token_def = *readUInt32();
      string_len = *readUInt32();
    } else if (token_ref >= TokenIndex::kMinTokenID) {
      continue;
    } else {
      string_len = token_ref;
    }

    auto str = std::string(readString(string_len), string_len);
    if (token_def > 0) {
      token_definitions.emplace_back( token_def, str);
    }
  }

  return token_definitions;
}

std::string AbstractSampleReader::readToken() {
  auto token_ref = *readUInt32();
  uint32_t string_len;
  uint32_t token_def = 0;

  if (token_ref == 0xffffffff) {
    token_def = *readUInt32();
    string_len = *readUInt32();
  } else if (token_ref >= TokenIndex::kMinTokenID) {
    return token_index_->resolveToken(token_ref);
  } else {
    string_len = token_ref;
  }

  auto token = std::string(readString(string_len), string_len);
  return token;
}

template <> double SampleReader<double>::readValue() {
  return fnord::IEEE754::fromBytes(*readUInt64());
}

}
}
}


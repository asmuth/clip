/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_SAMPLEWRITER_H
#define _FNORDMETRIC_METRICDB_SAMPLEWRITER_H
#include <fnord/util/binarymessagewriter.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnord {
namespace metric_service {
namespace disk_backend {
class TokenIndex;

class SampleWriter : public fnord::util::BinaryMessageWriter {
public:
  SampleWriter(TokenIndex* label_index);

  template <typename T>
  void writeValue(T value);

  void writeLabel(const std::string& key, const std::string& value);

protected:
  void writeToken(const std::string& token, bool force_indexing);
  TokenIndex* token_index_;
};

}
}
}

#endif

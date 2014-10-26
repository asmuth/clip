/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_BINARYFORMAT_H
#define _FNORDMETRIC_METRICDB_BINARYFORMAT_H
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnordmetric {
namespace metricdb {

/**
 * // http://tools.ietf.org/html/rfc5234
 *
 *   <table_header> :=
 *       <uint32_t>          // metric key size
 *       <bytes>             // metric key
 *       <uint64_t>          // generation
 *       <uint32_t>          // number of parent generations
 *       *<uint64_t>         // parent generations
 *
 *   <sample> :=
 *        <uint64_t>      // sample value
 *        *<label>        // sample labels
 *
 *   <label> :=
 *        <token>         // label key
 *        <token>         // label value
 *
 *   <token> :=
 *        <anonymous_token> | <token_definition> | <token_reference>
 *
 *   <anonymous_token> :=
 *        <uint32_t>      // string length (must be < 0xf0000000)
 *        <bytes>         // string bytes
 *
 *   <token_definition> :=
 *        <uint32_t>      // 0xffffffff
 *        <uint32_t>      // token id (must be 0xf0000000 < id < 0xffffffff)
 *        <uint32_t>      // string length (must be < 0xf0000000)
 *        <bytes>         // string bytes
 *
 *   <token_reference> :=
 *        <uint32_t>      // token id (must be 0xf0000000 < id < 0xffffffff)
 *
 */
class BinaryFormat {
public:

  struct TableHeader {
    std::string metric_key;
  };

  struct __attribute__((packed)) SampleHeader {
    uint64_t value;
  };

  struct __attribute__((packed)) FieldHeader {
    uint32_t key;
    uint32_t value_len;
  };

};

}
}

#endif

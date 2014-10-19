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

namespace fnordmetric {
namespace metricdb {

/**
 * // http://tools.ietf.org/html/rfc5234
 *
 *   <table_header> :=
 *       <uint64_t>          // first_timestamp
 *       <uint32_t>          // metric key size
 *       <bytes>             // metric key
 *       *<feld_definition>  // field definitions
 *
 *   <sample> :=
 *        <uint64_t>      // value
 *        *<field>        // fields
 *
 *   <field> :=
 *        <field_reference>
 *        <uint32_t>      // value len
 *        <bytes>         // value
 *
 *   <field_reference> :=
 *        <field_definition> | <field_id>
 *
 *   <field_id> :=
 *        <uint32_t>      // key (must be < 0xffffffff)
 *
 *   <field_definition> :=
 *        %xffffffff      // uint32_t
 *        <field_id>      // key
 *        <uint32_t>      // value len
 *        <bytes>         // value
 *
 */
class BinaryFormat {
public:

  struct __attribute__((packed)) TableHeader {
    uint64_t first_timestamp;
    uint32_t metric_key_size;
    char metric_key[0];
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

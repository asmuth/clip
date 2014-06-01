/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_RECORD_H
#define _FNORDMETRIC_RECORD_H

#include <stdlib.h>
#include <stdint.h>
#include "schema.h"

namespace fnordmetric {

/**
 * A record writer is a stateful object that can be used to unserialize one or
 * more records of the same schema.
 *
 * FIXPAUL documentation
 *
 * All methods on a record reader are threadsafe.
 */
class RecordReader {
public:
  explicit RecordReader(const Schema& schema);
  int64_t getIntegerField(const void* record, size_t field_index) const;
  double getFloatField(const void* reord, size_t field_index) const;
protected:
  std::vector<size_t> field_offsets_;
#ifndef NDEBUG
  std::vector<size_t> field_types_;
#endif
  const void* data_;
};

/**
 * A record writer is a stateful object that can be used to serialize one or
 * more records of the same schema. It must be initialized with a schema and
 * will allocate some memory to store a single record internally.
 *
 * The record data is build up by calling the respective setField method for
 * each field that is defined by the schema. You must take care to call the
 * correct setField method for each field, i.e. the one that matches the type
 * that is defined in the schema.
 *
 * After all fields are set you can call toBytes to retrieve a pointer to the
 * binary representation of the record. The returned pointer is valid until the
 * RecordWriter object is destructed or setField() or reset() is called.
 *
 * To write multiple records with the same RecordWriter object, call the reset()
 * method after each record.
 *
 * A record writer is not threadsafe! If you want to use it with multiple
 * threads you must take care to synchronize access in such a way that no two
 * threads call any method on the record writer object at the same time!
 */
class RecordWriter {
public:
  explicit RecordWriter(const Schema& schema);
  ~RecordWriter();

  /**
   * Set the n'th field (as defined by the schema) of the record to value. This
   * method will assert if the type of the n'th field (as defined by the schema)
   * is not a fnordmetric::IntegerField! Field indices are zero based.
   */
  void setIntegerField(size_t field_index, int64_t value);

  /**
   * Set the n'th field (as defined by the schema) of the record to value. This
   * method will assert if the type of the n'th field (as defined by the schema)
   * is not a fnordmetric::FloatField! Field indices are zero based.
   */
  void setFloatField(size_t field_index, double value);

  /**
   * Returns a pointer to the raw binary representation of this record. The
   * returned pointer is valid until the RecordWriter object is destructed or
   * setField() or reset() is called.
   */
  void toBytes(const void** data, size_t* size) const;

  /**
   * Reset the record writer so that a new record can be written.
   */
  void reset();

protected:
  void* alloc_;
  size_t alloc_size_;
  size_t min_size_;
  size_t last_byte_;
  std::vector<size_t> field_offsets_;
#ifndef NDEBUG
  std::vector<size_t> field_types_;
#endif
};


template<typename... T>
class TypedRecordWriter : public RecordWriter {
public:
  explicit TypedRecordWriter(const typename T::ValueType&... values);
protected:
  template<typename T1> void appendFields(T1 head);
  template<typename T1, typename... T2> void appendFields(T1 head, T2... tail);
};

}

#include "record_impl.h"
#endif

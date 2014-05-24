/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_RECORD_IMPL_H
#define _FNORDMETRIC_RECORD_IMPL_H

namespace fnordmetric {

template<typename... T>
RecordWriter<T...>::RecordWriter(const typename T::ValueType&... values) {
  appendFields(values...);
}

template<typename... T>
template<typename T1>
void RecordWriter<T...>::appendFields(T1 head) {
  appendField(head);
}

template<typename... T>
template <typename T1, typename... T2>
void RecordWriter<T...>::appendFields(T1 head, T2... tail) {
  appendField(head);
  appendFields(tail...);
}

}
#endif

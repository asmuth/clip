/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_RECORD_IMPL_H
#define _FNORDMETRIC_RECORD_IMPL_H

namespace fnordmetric {

template<typename... T>
TypedRecordWriter<T...>::TypedRecordWriter(
    const typename T::ValueType&... values) {
  appendFields(values...);
}

template<typename... T>
template<typename T1>
void TypedRecordWriter<T...>::appendFields(T1 head) {
  appendField(head);
}

template<typename... T>
template <typename T1, typename... T2>
void TypedRecordWriter<T...>::appendFields(T1 head, T2... tail) {
  appendField(head);
  appendFields(tail...);
}

}
#endif

/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
namespace fnord {
namespace json {

template <typename T>
void JSONOutputStream::addValue(const std::set<T>& value) {
  beginArray();

  int n = 0;
  for (const auto& elem : value) {
    if (n++ != 0) {
      addComma();
    }

    addValue(elem);
  }

  endArray();
}

template <typename T>
void JSONOutputStream::addValue(const std::vector<T>& value) {
  beginArray();

  int n = 0;
  for (const auto& elem : value) {
    if (n++ != 0) {
      addComma();
    }

    addValue(elem);
  }

  endArray();
}

}
}

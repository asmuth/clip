/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "stringutil.h"
#include "outputstream.h"

namespace fviz {

template <typename T1, typename T2>
std::string inspect(const std::pair<T1, T2>& value) {
  std::string str = "<";
  str += inspect(value.first);
  str += ", ";
  str += inspect(value.second);
  str += '>';
  return str;
}

template <typename T>
std::string inspect(const std::vector<T>& value) {
  std::string str = "[";
  for (auto iter = value.begin(); iter != value.end(); ++iter) {
    if (iter != value.begin()) {
      str += ", ";
    }

    str += inspect(*iter);
  }
  str += ']';
  return str;
}

template <typename T>
std::string inspect(const std::set<T>& value) {
  std::string str = "{";
  for (auto iter = value.begin(); iter != value.end(); ++iter) {
    if (iter != value.begin()) {
      str += ", ";
    }

    str += inspect(*iter);
  }
  str += '}';
  return str;
}

template <typename T>
std::string inspect(T* value) {
  return "@0x" + StringUtil::hexPrint(&value, sizeof(void*), false, true);
}

template <typename H, typename... T>
std::vector<std::string> inspectAll(H head, T... tail) {
  auto vec = inspectAll(tail...);
  vec.insert(vec.begin(), inspect(head));
  return vec;
}

template <typename H>
std::vector<std::string> inspectAll(H head) {
  std::vector<std::string> vec;
  vec.push_back(inspect(head));
  return vec;
}

template <typename... T>
void iputs(const char* fmt, T... values) {
  auto str = StringUtil::formatv(fmt, inspectAll(values...));
  str += "\n";
  OutputStream::getStdout()->write(str);
}

}


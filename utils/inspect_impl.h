/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "stringutil.h"
#include "outputstream.h"

namespace plotfx {

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


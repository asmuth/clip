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
#include "inspect.h"

namespace signaltk {

template <>
std::string inspect<bool>(const bool& value) {
  return value == true ? "true" : "false";
}

template <>
std::string inspect<int>(const int& value) {
  return std::to_string(value);
}

template <>
std::string inspect<unsigned int>(const unsigned int& value) {
  return std::to_string(value);
}

template <>
std::string inspect<unsigned long>(const unsigned long& value) {
  return std::to_string(value);
}

template <>
std::string inspect<unsigned long long>(const unsigned long long& value) {
  return std::to_string(value);
}

template <>
std::string inspect<unsigned char>(const unsigned char& value) {
  return std::to_string(value);
}

template <>
std::string inspect<long long>(
    const long long& value) {
  return std::to_string(value);
}

template <>
std::string inspect<long>(
    const long& value) {
  return std::to_string(value);
}

template <>
std::string inspect<unsigned short>(
    const unsigned short& value) {
  return std::to_string(value);
}

template <>
std::string inspect<float>(const float& value) {
  return std::to_string(value);
}

template <>
std::string inspect<double>(const double& value) {
  return std::to_string(value);
}

template <>
std::string inspect<std::string>(const std::string& value) {
  return value;
}

template <>
std::string inspect<char const*>(char const* const& value) {
  return std::string(value);
}

template <>
std::string inspect<void*>(void* const& value) {
  return "<ptr>";
}

template <>
std::string inspect<const void*>(void const* const& value) {
  return "<ptr>";
}

template <>
std::string inspect<std::exception>(const std::exception& e) {
  return e.what();
}

} // namespace signaltk


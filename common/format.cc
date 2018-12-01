/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include <sstream>
#include <iomanip>
#include "format.h"

namespace plotfx {

Formatter format_decimal_scientific(size_t precision) {
  Formatter f;
  f.format_number = [precision] (double v) -> std::string {
    std::stringstream s;
    s << std::scientific << std::setprecision(precision) << v;
    return s.str();
  };

  return f;
}

Formatter format_decimal_fixed(size_t precision) {
  Formatter f;
  f.format_number = [precision] (double v) -> std::string {
    std::stringstream s;
    s << std::fixed << std::setprecision(precision) << v;
    return s.str();
  };

  return f;
}

ReturnCode confgure_format(
    const plist::Property& prop,
    Formatter* formatter) {
  if (prop.size() < 1) {
    return ERROR_INVALID_ARGUMENT;
  }

  if (prop[0].data == "fixed") {
    *formatter = format_decimal_fixed(1); // FIXME
    return OK;
  }

  if (prop[0].data == "scientific") {
    *formatter = format_decimal_scientific(1); // FIXME
    return OK;
  }

  return OK;
}

} // namespace plotfx


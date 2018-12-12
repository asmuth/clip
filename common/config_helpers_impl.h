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
#pragma once
namespace plotfx {

template <typename T>
ReturnCode resolve_slot(
    const Slot<T>& slot,
    std::function<ReturnCode (const DimensionConfig&, const Value&, T*)> map,
    const DimensionMap& dimensions,
    const DataFrame& data,
    size_t data_idx,
    T* val) {
  if (slot.constant) {
    *val = *slot.constant;
    return OK;
  }

  if (!slot.key.empty()) {
    auto dimension = dimension_find(dimensions, slot.key);
    if (!dimension) {
      return ReturnCode::errorf("EARG", "dimension not found: $0", slot.key);
    }

    const auto& data_val = data_lookup(data, slot.key, data_idx);
    if (auto rc = map(*dimension, data_val, val); !rc) {
      return rc;
    }

    return OK;
  }

  return OK;
}

template <typename T>
ReturnCode configure_slot(
    const plist::Property& prop,
    DimensionMap* dimensions,
    Slot<T>* slot) {
  if (plist::is_value(prop) && prop.value.size() > 0 && prop.value[0] == '$') {
    slot->key = prop.value.substr(1);
    dimension_add(dimensions, slot->key);
  }

  return OK;
}

template <typename T>
ParserFn configure_slot(DimensionMap* dimensions, Slot<T>* slot) {
  return [dimensions, slot] (const plist::Property& prop) {
    return configure_slot(prop, dimensions, slot);
  };
}

} // namespace plotfx


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
#ifndef _FNORD_STDTYPES_H
#define _FNORD_STDTYPES_H
#include <ctime>
#include <deque>
#include <inttypes.h>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

namespace signaltk {

using String = std::string;
using WString = std::wstring;

template <typename T>
using ScopedPtr = std::unique_ptr<T>;

template <typename T>
using ScopedLock = std::unique_lock<T>;

template <typename T>
using Vector = std::vector<T>;

template <typename T>
using Set = std::set<T>;

template <typename T>
using List = std::list<T>;

template <typename T>
using Deque = std::deque<T>;

template <typename T>
using Function = std::function<T>;

template <typename T1, typename T2>
using Pair = std::pair<T1, T2>;

template <typename... T>
using Tuple = std::tuple<T...>;

template <typename T1, typename T2>
using HashMap = std::unordered_map<T1, T2>;

template <typename T>
using Stack = std::stack<T>;

} // namespace signaltk
#endif

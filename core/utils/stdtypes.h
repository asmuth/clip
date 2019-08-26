/**
 * This file is part of the "clip" project
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

namespace clip {

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

} // namespace clip
#endif

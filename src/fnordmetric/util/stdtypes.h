/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#ifndef _STX_STDTYPES_H
#define _STX_STDTYPES_H
#include <condition_variable>
#include <ctime>
#include <deque>
#include <inttypes.h>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using String = std::string;
using WString = std::wstring;
using UTF32String = std::string;

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

template <typename T1, typename T2>
using OrderedMap = std::map<T1, T2>;

template <typename T>
using Stack = std::stack<T>;

using StandardException = std::exception;

#endif

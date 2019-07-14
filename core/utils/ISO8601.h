/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth, Laura Schlimmer
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
#pragma once
#include "stdtypes.h"
#include "option.h"
#include "CivilTime.h"

class ISO8601 {
public:

  static Option<CivilTime> parse(const String& str);

  static bool isLeapYear(uint16_t year);

  static uint8_t daysInMonth(uint16_t year, uint8_t month);

};


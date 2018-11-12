/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include "utils/return_code.h"
#include "element.h"

namespace signaltk {

ReturnCode buildElement(
    const std::string& name,
    const plist::PropertyList& plist,
    std::unique_ptr<Element>* elem);

} // namespace signaltk


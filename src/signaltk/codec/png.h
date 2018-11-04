/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2017 Paul Asmuth
 *
 * signaltk is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include <signaltk.h>
#include <signaltk/core/image.h>

namespace signaltk {

Status pngWriteImageFile(
    const Image& image,
    const std::string& filename);

} // namespace signaltk


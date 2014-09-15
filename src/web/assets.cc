/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <unordered_map>
#include <string>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/web/assets.h>

namespace fnordmetric {
namespace web {

static std::unordered_map<std::string, std::pair<const unsigned char*, size_t>>
    asset_files;

Assets::AssetFile::AssetFile(
    const std::string& name,
    const unsigned char* data,
    size_t size) {
  asset_files.emplace(name, std::make_pair(data, size));
}

std::string Assets::getAsset(const std::string& filename) {
  const auto asset = asset_files.find(filename);

  if (asset != asset_files.end()) {
    const auto& data = asset->second;
    return std::string((const char*) data.first, data.second);
  }

  RAISE(util::RuntimeException, "asset not found: %s", filename.c_str());
}

}
}

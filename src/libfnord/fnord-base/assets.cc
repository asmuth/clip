/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include "fnord-base/assets.h"
#include "fnord-base/exception.h"
#include "fnord-base/io/inputstream.h"

namespace fnord {

Assets::AssetMap* Assets::globalMap() {
  static AssetMap map;
  return &map;
}

Assets::AssetFile::AssetFile(
    const std::string& name,
    const unsigned char* data,
    size_t size) {
  Assets::registerAsset(name, data, size);
}

void Assets::registerAsset(
    const std::string& filename,
    const unsigned char* data,
    size_t size) {
  auto asset_map = Assets::globalMap();
  std::lock_guard<std::mutex> lock_holder(asset_map->mutex);
  asset_map->assets.emplace(filename, std::make_pair(data, size));
}

std::string Assets::getAsset(const std::string& filename) {
#ifndef _NDEBUG
  auto dev_asset_path = getenv("DEV_ASSET_PATH");

  if (dev_asset_path != nullptr) {
    // FIXPAUL check that file exists
    auto file = FileInputStream::openFile(
        std::string(dev_asset_path) + "/" + filename);

    std::string asset_str;
    file->readUntilEOF(&asset_str);
    return asset_str;
  }
#endif

  auto asset_map = Assets::globalMap();
  std::lock_guard<std::mutex> lock_holder(asset_map->mutex);
  const auto asset = asset_map->assets.find(filename);

  if (asset != asset_map->assets.end()) {
    const auto& data = asset->second;
    return std::string((const char*) data.first, data.second);
  }

  RAISE(kRuntimeError, "asset not found: %s", filename.c_str());
}

} // namespace fnord


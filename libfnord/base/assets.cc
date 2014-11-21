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
#include <fnord/base/assets.h>
#include <fnord/base/exception.h>
#include <fnordmetric/util/inputstream.h>
#include <cstdlib>

namespace fnord {

static std::unordered_map<
    std::string, std::pair<const unsigned char*, size_t>> asset_lib;
static std::mutex asset_lib_mutex;

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
  std::lock_guard<std::mutex> lock_holder(asset_lib_mutex);
  asset_lib.emplace(filename, std::make_pair(data, size));
}

std::string Assets::getAsset(const std::string& filename) {
#ifndef _NDEBUG
  auto dev_asset_path = getenv("DEV_ASSET_PATH");

  if (dev_asset_path != nullptr) {
    // FIXPAUL check that file exists
    auto file = fnordmetric::util::FileInputStream::openFile(
        std::string(dev_asset_path) + "/" + filename);

    std::string asset_str;
    file->readUntilEOF(&asset_str);
    return asset_str;
  }
#endif

  std::lock_guard<std::mutex> lock_holder(asset_lib_mutex);
  const auto asset = asset_lib.find(filename);

  if (asset != asset_lib.end()) {
    const auto& data = asset->second;
    return std::string((const char*) data.first, data.second);
  }

  RAISE(kRuntimeError, "asset not found: %s", filename.c_str());
}

} // namespace fnord


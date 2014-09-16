/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/web/assets.h>
#include <asset_bundle.cc>
#include <cstdlib>

namespace fnordmetric {
namespace web {

std::unordered_map<std::string, std::pair<const unsigned char*, size_t>>*
    Assets::global_map() {
  static std::unordered_map<
      std::string, std::pair<const unsigned char*, size_t>> map;
  return &map;
}

Assets::AssetFile::AssetFile(
    const std::string& name,
    const unsigned char* data,
    size_t size) {
  Assets::global_map()->emplace(name, std::make_pair(data, size));
}

std::string Assets::getAsset(const std::string& filename) {
#ifndef _NDEBUG
  auto dev_asset_path = std::getenv("DEV_ASSET_PATH");

  if (dev_asset_path != nullptr) {
    // FIXPAUL check that file exists
    auto file = util::FileInputStream::openFile(
        std::string(dev_asset_path) + "/" + filename);

    std::string asset_str;
    file->readUntilEOF(&asset_str);
    return asset_str;
  }
#endif

  auto asset_files = Assets::global_map();
  const auto asset = asset_files->find(filename);

  if (asset != asset_files->end()) {
    const auto& data = asset->second;
    return std::string((const char*) data.first, data.second);
  }

  RAISE(util::RuntimeException, "asset not found: %s", filename.c_str());
}

}
}

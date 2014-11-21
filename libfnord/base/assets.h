/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_WEB_ASSETS_H
#define _FNORDMETRIC_WEB_ASSETS_H
#include <string>
#include <unordered_map>
#include <vector>

namespace fnordmetric {
namespace util {

class Assets {
public:
  class AssetFile {
  public:
    AssetFile(
        const std::string& name,
        const unsigned char* data,
        size_t size);
  };

  static void registerAsset(
      const std::string& filename,
      const unsigned char* data,
      size_t size);

  static std::string getAsset(const std::string& filename);

protected:
  static std::unordered_map<
      std::string, std::pair<const unsigned char*, size_t>>* global_map();
};

}
}
#endif

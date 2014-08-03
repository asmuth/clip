/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/web/assets.h>
#include <asset_index.html.h>
#include <fnordmetric_web.css.h>

namespace fnordmetric {
namespace web {

#define TRY_FILE(F,T) \
  if (filename == F) { \
    return std::string((const char *) T, sizeof(T)); \
  }

std::string Assets::getAsset(const std::string& filename) {
  TRY_FILE("index.html", assets_index_html);
  TRY_FILE("fnordmetric-web.css", assets_fnordmetric_web_css);
  RAISE(util::RuntimeException, "asset not found: %s\n", filename.c_str());
}

}
}

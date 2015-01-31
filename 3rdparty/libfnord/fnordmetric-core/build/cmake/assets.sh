#/bin/bash
set -e

mkdir -p stage/src
echo "#include <util/assets.h>" > stage/src/asset_bundle.cc

asset_file() {
  (cd ../../.. && cat $@ | xxd -i)
  echo "};"
}

asset_uniq() {
  echo "static const unsigned char __$1_data[] = {"
}

asset_name() {
  echo "static fnordmetric::util::Assets::AssetFile __$1(\"$2\", __$1_data, sizeof(__$1_data));"
};

(
  asset_uniq "fnordmetric_js_fnordmetric_js"
  asset_file "fnordmetric-js/fnordmetric.js"
  asset_name "fnordmetric_js_fnordmetric_js" "fnordmetric-js/fnordmetric.js"
) >> stage/src/asset_bundle.cc

(
  asset_uniq "fnordmetric_webui_fnordmetric_webui_css"
  asset_file "fnordmetric-webui/fnordmetric-webui.css"
  asset_name "fnordmetric_webui_fnordmetric_webui_css" "fnordmetric-webui/fnordmetric-webui.css"
) >> stage/src/asset_bundle.cc

(
  asset_uniq "fnordmetric_webui_fnordmetric_webui_html"
  asset_file "fnordmetric-webui/fnordmetric-webui.html"
  asset_name "fnordmetric_webui_fnordmetric_webui_html" "fnordmetric-webui/fnordmetric-webui.html"
) >> stage/src/asset_bundle.cc

(
  asset_uniq "fnordmetric_webui_fnordmetric_webui_js"
  asset_file "fnordmetric-webui/fnordmetric-webui.js"
  asset_name "fnordmetric_webui_fnordmetric_webui_js" "fnordmetric-webui/fnordmetric-webui.js"
) >> stage/src/asset_bundle.cc

(
  asset_uniq "fnordmetric_webui_fnordmetric_favicon_ico"
  asset_file "fnordmetric-webui/fnordmetric-favicon.ico"
  asset_name "fnordmetric_webui_fnordmetric_favicon_ico" "fnordmetric-webui/fnordmetric-favicon.ico"
) >> stage/src/asset_bundle.cc

(
  asset_uniq "fnordmetric_webui_fontawesome_woff"
  asset_file "fnordmetric-webui/fontawesome.woff"
  asset_name "fnordmetric_webui_fontawesome_woff" "fnordmetric-webui/fontawesome.woff"
) >> stage/src/asset_bundle.cc

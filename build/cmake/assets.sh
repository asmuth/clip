#/bin/bash
mkdir -p stage/src
echo "#include <fnordmetric/web/assets.h>" > stage/src/asset_bundle.cc

asset_file() {
  (cd ../.. && cat $@ | xxd -i)
  echo "};"
}

asset_uniq() {
  echo "static const unsigned char __$1_data[] = {"
}

asset_name() {
  echo "static fnordmetric::web::Assets::AssetFile __$1(\"$2\", __$1_data, sizeof(__$1_data));"
};

(
  asset_uniq "fnordmetric_js_fnordmetric_js"
  asset_file "contrib/fnordmetric-js/fnordmetric.js"
  asset_name "fnordmetric_js_fnordmetric_js" "fnordmetric-js/fnordmetric.js"
) >> stage/src/asset_bundle.cc

(
  asset_uniq "fnordmetric_js_fnordmetric_editor_js"
  asset_file "contrib/fnordmetric-js/fnordmetric-editor.js"
  asset_name "fnordmetric_js_fnordmetric_editor_js" "fnordmetric-js/fnordmetric-editor.js"
) >> stage/src/asset_bundle.cc

(
  asset_uniq "fnordmetric_webui_fnordmetric_webui_css"
  asset_file "contrib/fnordmetric-webui/fnordmetric-webui.css"
  asset_name "fnordmetric_webui_fnordmetric_webui_css" "fnordmetric-webui/fnordmetric-webui.css"
) >> stage/src/asset_bundle.cc

(
  asset_uniq "fnordmetric_webui_fnordmetric_webui_html"
  asset_file "contrib/fnordmetric-webui/fnordmetric-webui.html"
  asset_name "fnordmetric_webui_fnordmetric_webui_html" "fnordmetric-webui/fnordmetric-webui.html"
) >> stage/src/asset_bundle.cc

(
  asset_uniq "fnordmetric_webui_fnordmetric_webui_js"
  asset_file "contrib/fnordmetric-webui/fnordmetric-webui.js"
  asset_name "fnordmetric_webui_fnordmetric_webui_js" "fnordmetric-webui/fnordmetric-webui.js"
) >> stage/src/asset_bundle.cc

(
  asset_uniq "fnordmetric_webui_fnordmetric_favicon_ico"
  asset_file "contrib/fnordmetric-webui/fnordmetric-favicon.ico"
  asset_name "fnordmetric_webui_fnordmetric_favicon_ico" "fnordmetric-webui/fnordmetric-favicon.ico"
) >> stage/src/asset_bundle.cc

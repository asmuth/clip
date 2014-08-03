#/bin/bash

mkdir -p stage/assets

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
  asset_uniq "fnordmetric_index_html"
  asset_file "assets/index.html"
  asset_name "fnordmetric_index_html" "index.html"
) > stage/assets/asset_index.html.c

(
  asset_uniq "fnordmetric_web_css"
  asset_file "assets/codemirror.css" "assets/fnordmetric-web.css"
  asset_name "fnordmetric_web_css" "fnordmetric-web.css"
) > stage/assets/asset_fnordmetric_web.css.c

(
  asset_uniq "fnordmetric_web_js"
  asset_file "assets/codemirror.min.js" "assets/fnordmetric-js"
  asset_name "fnordmetric_web_js" "fnordmetric-web.js"
) > stage/assets/asset_fnordmetric_web.js.c

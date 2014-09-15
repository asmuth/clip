#/bin/bash

mkdir -p assets
>stage/asset_bundle.c

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
  asset_uniq "fnordmetric_js"
  asset_file "contrib/fnordmetric-js/fnordmetric.js"
  asset_name "fordmetric_js" "fnordmetric.js"
) >> stage/asset_bundle.c

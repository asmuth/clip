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
  asset_uniq "fnordmetric_js"
  asset_file "contrib/fnordmetric-js/fnordmetric.js"
  asset_name "fnordmetric_js" "fnordmetric.js"
) >> stage/src/asset_bundle.cc

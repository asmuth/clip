#/bin/bash
set -e

assets_init() {
  if [[ -z $ASSETS_FILE ]]; then
    echo "ASSETS_FILE env var must be set"
    exit 1
  fi;

  echo "#include <fnord-base/assets.h>" > $ASSETS_FILE
}

assets_add() {
  uniq="blah"

  (
    echo "static const unsigned char __${uniq}_data[] = {"
    (cat $1 | xxd -i)
    echo "};"
    echo "static fnordmetric::util::Assets::AssetFile __${uniq}(\"$2\", __${uniq}_data, sizeof(__${uniq}_data));"
  ) >> $assets_file
}

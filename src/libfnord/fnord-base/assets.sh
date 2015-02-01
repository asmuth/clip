#/bin/bash
set -e

fn_assets_init() {
  if [[ -z $ASSETS_FILE ]]; then
    echo "ASSETS_FILE env var must be set"
    exit 1
  fi;

  echo "#include <fnord-base/assets.h>" > $ASSETS_FILE
}

# $1 => asset logical/fake path
# $2 => asset real path
fn_assets_add() {
  if [[ -z $ASSETS_FILE ]]; then
    echo "ASSETS_FILE env var must be set"
    exit 1
  fi;

  uniq=$(echo $ASSETS_FILE | sed -e 's/[^a-zA-Z0-9]/_/g')

  (
    echo "static const unsigned char __${uniq}_data[] = {"
    (cat $2 | xxd -i)
    echo "};"
    echo "static fnord::Assets::AssetFile __${uniq}(\"$1\", __${uniq}_data, sizeof(__${uniq}_data));"
  ) >> $ASSETS_FILE
}

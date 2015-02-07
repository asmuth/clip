#/bin/bash
# usage:
#   echo "myasset.ext:/path/to/real/myasset.ext" | assets.sh output_asset_pack.cc
set -e

ASSETS_FILE=$1

if [[ -z $ASSETS_FILE ]]; then
  echo "usage: $0 [output_asset_pack.cc]"
  exit 1
fi;

echo "#include <fnord-base/assets.h>" > $ASSETS_FILE

while read line; do
  logical_path=$(echo $line | sed -e 's/:.*//')
  real_path=$(echo $line | sed -e 's/.*://')
  uniq=$(echo $logical_path | sed -e 's/[^a-zA-Z0-9]/_/g')

  echo "Packing asset: $logical_path"

  (
    echo "static const unsigned char __${uniq}_data[] = {"
    (cat $real_path | xxd -i)
    echo "};"
    echo "static fnord::Assets::AssetFile __${uniq}(\"$logical_path\", __${uniq}_data, sizeof(__${uniq}_data));"
  ) >> $ASSETS_FILE
done

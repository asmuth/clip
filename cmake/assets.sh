#/bin/bash
set -e
source $(dirname $0)/../src/libfnord/fnord-base/assets.sh

fn_assets_init
fn_assets_add "fnord/3rdparty/codemirror.js" "src/libfnord/fnord-webcomponents/3rdparty/codemirror.js" # 

#(
#  asset_uniq "fnordmetric_webui_fnordmetric_webui_css"
#  asset_file "fnordmetric-webui/fnordmetric-webui.css"
#  asset_name "fnordmetric_webui_fnordmetric_webui_css" "fnordmetric-webui/fnordmetric-webui.css"
#) >> stage/src/asset_bundle.cc
#
#(
#  asset_uniq "fnordmetric_webui_fnordmetric_webui_html"
#  asset_file "fnordmetric-webui/fnordmetric-webui.html"
#  asset_name "fnordmetric_webui_fnordmetric_webui_html" "fnordmetric-webui/fnordmetric-webui.html"
#) >> stage/src/asset_bundle.cc
#
#(
#  asset_uniq "fnordmetric_webui_fnordmetric_webui_js"
#  asset_file "fnordmetric-webui/fnordmetric-webui.js"
#  asset_name "fnordmetric_webui_fnordmetric_webui_js" "fnordmetric-webui/fnordmetric-webui.js"
#) >> stage/src/asset_bundle.cc
#
#(
#  asset_uniq "fnordmetric_webui_fnordmetric_favicon_ico"
#  asset_file "fnordmetric-webui/fnordmetric-favicon.ico"
#  asset_name "fnordmetric_webui_fnordmetric_favicon_ico" "fnordmetric-webui/fnordmetric-favicon.ico"
#) >> stage/src/asset_bundle.cc
#
#(
#  asset_uniq "fnordmetric_webui_fontawesome_woff"
#  asset_file "fnordmetric-webui/fontawesome.woff"
#  asset_name "fnordmetric_webui_fontawesome_woff" "fnordmetric-webui/fontawesome.woff"
#) >> stage/src/asset_bundle.cc

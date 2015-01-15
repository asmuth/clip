package "fnord/webui/examples"

executable "webchat" {
  src = [
    "webchat.cc"
  ]
  link = [
    ":webchat_assets",
    "fnord/webui:base",
    "libx0/http:server",
    "//:pthread"
  ]
}

asset_bundle "webchat_assets" {
  src = [
    "webchat.css",
    "webchat_layout.tpl",
    "webchat_chatwindow.tpl"
  ]
  link = [
    "3rdparty/jquery.js",
    "3rdparty/bootstrap.css"
  ]
}


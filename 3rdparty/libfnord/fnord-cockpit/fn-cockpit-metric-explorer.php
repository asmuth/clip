<?php

function fn_render_metric_explorer($rpc_url) {
  $base_path = dirname(__FILE__);
?>

<style type="text/css">
  <?php
    include($base_path . "/metric-explorer/fn-metric-explorer.css");
  ?>
</style>

<?php
  include($base_path . "/metric-explorer/metric-explorer-list.html");
  include($base_path . "/metric-explorer/metric-explorer-preview.html");
?>

<div id="metric-explorer-viewport" style="height: 100%;">
</div>

<script type='text/javascript'>
  var baseUrl = "<?= $rpc_url ?>";
  var views = {
    "metrics" : "<fn-metric-explorer-list></fn-metric-explorer-list>",
    "metric" : "<fn-metric-explorer-preview></fn-metric-explorer-preview>",
    "search" : "<fn-metric-explorer-list></fn-metric-explorer-list>"
  }

  var routes = {
    "metrics": "fn-metric-explorer-list",
    "metric": "fn-metric-explorer-preview",
    "search": "fn-metric-explorer-list"
  };

  var default_route = "metrics";

  function openUrl(raw_url, push_state) {
    var url = parseUrlQueryString(raw_url);
    var query_params = url["query_params"];

    var view_name = routes[url["path"]];

    if (view_name == undefined) {
      alert("no route found for: " + url["path"]);
      return;
    }

    if (typeof push_state == "undefined" || push_state === true) {
      window.history.pushState({url: raw_url}, "", "#" + raw_url);
    }

    var viewport = document.querySelector("#metric-explorer-viewport");
    var view = document.createElement(view_name);

    raw_url = "#" + raw_url;
    if (view.getAttribute('data-url') != raw_url) {
      view.setAttribute("data-url", raw_url);
    }
    viewport.innerHTML = "";
    viewport.appendChild(view);

  };


  function parseUrlQueryString(qstr) {
    if (qstr == null) {return;}
    var path;
    var query_params = {};

    if (qstr.indexOf("?") >= 0) {
      path = qstr.substr(0, qstr.indexOf("?"));
      path = path.replace("#", "");

      var params_str = qstr.substr(qstr.indexOf("?") + 1);
      var raw_params = params_str.split('&');

      /* set first param which defines view's view (metric, search ...) */
      var param = raw_params[0].split('=');
      query_params.innerView = decodeURIComponent(param[0]);
      query_params.innerViewValue = decodeURIComponent(param[1]);

      for (var i = 1; i < raw_params.length; i++) {
        var param = (raw_params[i].split('=') != "undefined") ? 
          raw_params[i].split('=') : "";
        if (param[0] != "undefined") {
          query_params[decodeURIComponent(param[0])] =
             (param[1] != "undefined") ? 
             decodeURIComponent(param[1]) : "";
        }
      }

    } else {
      path = qstr != "undefined" ? qstr : "";
    }
    return {
      "path": path,
      "query_params": query_params
    }
  };

  function setUrlFromQueryString(hash, query_params, push_state) {
    if (hash === undefined || hash === "undefined") {
      window.location.hash = "";
      return;
    }
    var path = "#" + hash;

    if ("innerView" in query_params && query_params.innerView != undefined) {
      path += "?" + encodeURIComponent(query_params.innerView) + "=";
      path +=
        encodeURIComponent(query_params.innerViewValue);

      for (var param in query_params) {
        if (param != "innerView" && 
            param != "innerViewValue" &&
            query_params[param] != undefined &&
            query_params[param].length > 0) {

          path += 
            "&" + encodeURIComponent(param) +
            "=" + encodeURIComponent(query_params[param]);
        }
      }
    }

    if (push_state) {
      window.history.pushState({url:path}, "#", path);
    }
    window.location.hash = path;
    return path;
  }

  window.addEventListener('fn-ready' ,function() {
    var fragment = window.location.hash;
    if (fragment) {
      openUrl(fragment.substring(1));
    } else {
      openUrl(default_route);
    }

  }, false)
</script>

<?php
  }
?>



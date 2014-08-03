FnordMetric = (function() {
  var editor = undefined;

  var queryEditorActivateTab = function() {
    var tab_lis = document.getElementsByClassName("query_editor_result_tab");
    for (var i = 0; i < tab_lis.length; ++i) {
      if (tab_lis[i].getAttribute("data-tab") ==
          this.getAttribute("data-tab")) {
        tab_lis[i].className = "query_editor_result_tab active";
      } else {
        tab_lis[i].className = "query_editor_result_tab";
      }
    }

    var tabs = document.getElementsByClassName("query_editor_result_tab_content");
    for (var i = 0; i < tabs.length; ++i) {
      if (tabs[i].getAttribute("data-tab") == this.getAttribute("data-tab")) {
        tabs[i].className = "query_editor_result_tab_content active";
      } else {
        tabs[i].className = "query_editor_result_tab_content";
      }
    }
  };

  // FIXPAUL what is xss?
  var renderQueryResults = function(results) {
    var tabs = "<ul class='ui_tabs'>";
    for (var i = 0; i < results.charts.length; ++i) {
      tabs += "<li class='query_editor_result_tab' data-tab='chart" + i + "'><a>Chart</a></li>";
    }
    for (var i = 0; i < results.tables.length; ++i) {
      tabs += "<li class='query_editor_result_tab' data-tab='table" + i + "'><a>Table</a></li>";
    }
    tabs += "</ul>";

    var results_html = "<div>" + tabs;

    for (var i = 0; i < results.charts.length; ++i) {
      results_html += "<div class='query_editor_result_tab_content' data-tab='chart" + i + "'>";
      results_html += "<div class='chart'>";
      results_html += results.charts[i].svg;
      results_html += "</div>";
      results_html += "</div>";
    }

    for (var i = 0; i < results.tables.length; ++i) {
      results_html += "<div class='query_editor_result_tab_content' data-tab='table" + i + "'>";
      var result = results.tables[i];
      results_html += "<table><tbody>";

      results_html += "<tr>";
      for (j = 0; j < result.columns.length; ++j) {
        results_html += "<th>" + result.columns[j] + "</th>";
      }
      results_html += "</tr>";

      var j = 0;
      for (; j < result.rows.length; ++j) {
        var row = result.rows[j];
        results_html += "<tr>";
        for (k = 0; k < row.length; ++k) {
          results_html += "<td>" + row[k] + "</td>";
        }
        results_html += "</tr>";
      }

      for (; j < 20; ++j) {
        results_html += "<tr>";
        for (k = 0; k < result.columns.length; ++k) {
          results_html += "<td></td>";
        }
        results_html += "</tr>";
      }


      results_html += "</tbody></table>";
      results_html += "</div>";
    }

    document.getElementById("query_editor_results").innerHTML =
        results_html + "</div>";

    var tab_elems = document.getElementsByClassName("query_editor_result_tab");
    for (var i = 0; i < tab_elems.length; ++i) {
      tab_elems[i].onclick = queryEditorActivateTab;

      if (i == 0) {
        tab_elems[i].onclick();
      }
    }
  }

  var executeQuery = function() {
    document.getElementById("query_editor_results").innerHTML = "";

    if (typeof editor == undefined) {
      return;
    }

    var query = editor.getValue();
    if (query.length == 0) {
      return;
    }

    var xmlHttp = null;
    xmlHttp = new XMLHttpRequest();
    xmlHttp.open("POST", "/query", false);
    xmlHttp.send(query);

    console.log(xmlHttp.responseText);
    var response = JSON.parse(xmlHttp.responseText);

    var error_div = document.getElementById("query_editor_error");
    if (response.status == "success") {
      error_div.className = "";
      renderQueryResults(response);
    } else {
      error_div.className = "visible";
      error_div.innerHTML = "<b>Error:</b> " + response.error;
    }
  };

  var renderQueryView = function() {
    var navbar =
      "<div class='navbar'>" +
        "<a id='query_editor_executebtn' class='button navbutton'>Execute</a>" +
        "<h2>Unnamed Query</h2>" +
      "</div>";

    document.getElementById("wrap").innerHTML =
        navbar +
        "<div id='query_editor'>" +
          "<textarea id='query_editor_textarea'>DRAW BAR CHART;\nDRAW LEFT AXIS;\n\nSELECT\n\t'fnord' as series,\n\t5 as x,\n\t10 as y,\n\t4 as z;</textarea>" +
        "</div>" +
        "<div class='headbar small'>Query executed in...</div>" +
        "<div id='query_editor_error'></div>" +
        "<div id='query_editor_results'></div>";

    editor = CodeMirror.fromTextArea(
        document.getElementById("query_editor_textarea"),
        {
          mode: "sql",
          lineNumbers: true
        });

    document.getElementById("query_editor_executebtn").onclick = executeQuery;
  };

  var render = function() {
    renderQueryView();
  };

  return {
    "render": render,
  };
})();

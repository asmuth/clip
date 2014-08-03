FnordMetric = (function() {
  var editor = undefined;

  // FIXPAUL what is xss?
  var renderQueryResults = function(results) {
    var tabs = "<ul class='ui_tabs'>";
    for (i = 0; i < results.tables.length; ++i) {
      tabs += "<li class='active'><a>Table</a></li>";
    }
    tabs += "</ul>";

    var results_html = tabs;
    for (i = 0; i < results.tables.length; ++i) {
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
    }

    document.getElementById("query_editor_results").innerHTML = results_html;
  }

  var executeQuery = function() {
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

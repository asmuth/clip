FnordMetric = (function() {
  var editor = undefined;

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
    } else {
      error_div.className = "visible";
      error_div.innerHTML = response.error;
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
          "<textarea id='query_editor_textarea'></textarea>" +
        "</div>" +
        "<div id='query_editor_error'></div>";

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

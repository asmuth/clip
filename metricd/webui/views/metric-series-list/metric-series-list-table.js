var FnordMetricMetricSeriesListTable = function(table) {
  'use strict';

  this.render = function(series) {
    var tbody = table.querySelector("tbody");
    for (var i = 0; i < series.length; i++) {
      /** skip total summary row **/
      if (i == 0) {
        continue;
      }

      renderRow(series[i], tbody)
    }
  }

  var renderRow = function(series, tbody) {
    var tr = document.createElement("tr");
    var html = [];

    /** series id **/
    renderSeriesIdCell(series.series_id, tr);

    /** sparkline **/
    renderSparklineCell(series, tr);

    /** values **/
    renderSummariesCell(series.summaries, tr);

    /** context menu icon **/
    renderContextMenuIconCell(series.series_id, tr);

    /** watch right click **/
    tr.addEventListener("contextmenu", function(e) {
      //check if right click
      if (e.which) {
        if (e.which != 3) {
          return false;
        }
      } else if (e.button) { //IE, Opera
        if (e.button != 2) {
          return false;
        }
      }

      alert("render context menu");
      e.preventDefault();
      return false;
    });


    tbody.appendChild(tr);
  }

  var renderSeriesIdCell = function(id, tr) {
    var td = document.createElement("td");
    td.innerHTML = id;

    var eye_icon_elem = document.createElement("div");
    var eye_icon = document.createElement("i");
    eye_icon.className = "fa fa-eye";
    eye_icon_elem.appendChild(eye_icon);
    td.appendChild(eye_icon_elem);

    tr.appendChild(td);
  }

  var renderSparklineCell = function(series, tr) {
    var td = document.createElement("td");
    renderSparkline(td);

    tr.appendChild(td);
  }

  //TODO
  var renderSparkline = function(td_elem) {
    td_elem.innerHTML = "sparkline";

  }

  var renderSummariesCell = function(summaries, tr) {
    var sum;
    var value_stats = [];
    summaries.forEach(function(s) {
      switch (s.summary) {
        case "sum":
          sum = s.value;
          break;

        default:
          value_stats.push(s.value);
          break;
     }
    });

    var td = document.createElement("td");
    td.className = "values";
    td.innerHTML = ["<div class='total'>", sum || "-",
      "</div><div class='value_stats'>",
      value_stats.join(" "), "</div>"].join("");

    tr.appendChild(td);
  }

  var renderContextMenuIconCell = function(series_id, tr) {
    var td = document.createElement("td");
    td.className = "align_center context_menu_icon";

    var menu_icon = document.createElement("i");
    menu_icon.className = "fa fa-angle-down";

    menu_icon.addEventListener("click", function(e) {
      renderContextMenu(series_id, e);
    }, false);

    td.appendChild(menu_icon);
    tr.appendChild(td);
  }

  var renderContextMenu = function(series_id, ev) {
    //FIXME
    var menu = document.querySelector(".context_menu");
    menu.style.left = ev.clientX + "px";
    menu.style.top = ev.clientY + "px";
    menu.classList.add("active");
  }

}

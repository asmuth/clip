var FnordMetricMetricSeriesListTable = function(table) {
  'use strict';

  this.render = function(series) {
    for (var i = 0; i < series.length; i++) {
      /** skip total summary row **/
      if (i == 0) {
        continue;
      }

      renderRow(series[i])
    }
  }

  var renderRow = function(series) {
    var tr = document.createElement("tr");
    var html = [];

    /** series id **/
    var series_id_td = document.createElement("td");
    series_id_td.innerHTML = series.series_id;

    var eye_icon_elem = document.createElement("div");
    var eye_icon = document.createElement("i");
    eye_icon.className = "fa fa-eye";
    eye_icon_elem.appendChild(eye_icon);
    series_id_td.appendChild(eye_icon_elem);

    tr.appendChild(series_id_td);

    /** sparkline **/
    var sparkline_td = document.createElement("td");
    renderSparkline(sparkline_td);

    tr.appendChild(sparkline_td);

    /** values **/
    var sum;
    var value_stats = [];
    series.summaries.forEach(function(s) {
      switch (s.summary) {
        case "sum":
          sum = s.value;
          break;

        default:
          value_stats.push(s.value);
          break;
     }
    });

    var values_td = document.createElement("td");
    values_td.className = "values";
    values_td.innerHTML = ["<div class='total'>", sum || "-",
      "</div><div class='value_stats'>",
      value_stats.join(" "), "</div>"].join("");

    tr.appendChild(values_td);

    /** context menu icon **/
    var menu_icon_td = document.createElement("td");
    menu_icon_td.className = "align_center context_menu";

    var menu_icon = document.createElement("i");
    menu_icon.className = "fa fa-angle-down";

    menu_icon.addEventListener("click", function(e) {
      alert("display context menu");
    }, false);

    menu_icon_td.appendChild(menu_icon);
    tr.appendChild(menu_icon_td);

    table.querySelector("tbody").appendChild(tr);
  }

  //TODO
  var renderSparkline = function(td_elem) {
    td_elem.innerHTML = "sparkline";

  }

}

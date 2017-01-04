var FnordMetricMetricSeriesListTable = function(table) {
  'use strict';

  this.render = function(series) {
    var tbody = table.querySelector("tbody");
    series.forEach(function(s) {
      /** skip summary row **/
      if (s.tags && s.tags.indexOf("summary") > -1) {
        return;
      }

      renderRow(s, tbody)
    });
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

      //alert("render context menu");
      renderContextMenu(series.series_id, e);
      e.preventDefault();
      //return false;
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
    renderSparkline(series, td);

    tr.appendChild(td);
  }

  var renderSparkline = function(series, td) {
    var sparkline_cfg = {
      series: [
        {
          values: series.values
        }
      ]
    };

    var svg = document.querySelector("svg.sparkline_tpl").cloneNode(true);
    td.appendChild(svg);
    td.className = "sparkline";

    FnordMetricMetricSeriesListSparkline.render(svg, sparkline_cfg);
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

    var value_stats_line = value_stats.length > 0 ?
      value_stats.join(" ") :
      " - ";

    var td = document.createElement("td");
    td.className = "values";
    td.innerHTML = ["<div class='total'>", sum || "-",
      "</div><div class='value_stats'>", value_stats_line, "</div>"].join("");

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
    ev.stopPropagation();
    var menu = document.querySelector(".context_menu");
    menu.classList.add("active");

    /** set left position **/
    if (window.innerWidth - ev.clientX < menu.offsetWidth) {
      menu.style.left = (ev.clientX - menu.offsetWidth + 19) + "px";
      menu.setAttribute("data-pointer", "right");

    } else if (ev.clientX < menu.offsetWidth / 2) {
      menu.style.left = ev.clientX - 16 + "px";
      menu.setAttribute("data-pointer", "left");

    } else {
      menu.style.left = ev.clientX - menu.offsetWidth / 2 + "px";
      menu.setAttribute("data-pointer", "center");

    }

    /** set top position **/
    menu.style.top = ev.clientY + window.scrollX + 5 + "px";
  }


  var hideContextMenu = function() {
    document.querySelector(".context_menu").classList.remove("active");
  }

  document.addEventListener("click", function(e) {
    hideContextMenu();
  }, false);
}

var FnordMetricMetricSeriesListSparkline = (function() {

  /**
    * @param elem the html elem
    * @param cfg (Object) consists of multiple series object, each with 
    * values: an array of float values for the y axis
    * min (optinal): a float determining the min value
    * max(optinal): a float determining the max value
    **/
  var render = function(elem, cfg) {
    var height = getDimension('height', elem);
    var width = getDimension("width", elem);

    renderGrid(elem, height, width);

    var path_elems = elem.querySelectorAll(".lines path");

    for (var i = 0; i < cfg.series.length; i++) {
      if (path_elems.length - 1 < i) {
        break;
      }

      renderPath(cfg.series[i], height, width, path_elems[i]);
    }

    elem.style.height = height + "px";
    elem.style.width = width + "px";
  }

  var renderGrid = function(elem, height, width) {
    /** render x axis in the middle of the sparkline **/
    var x_axis = elem.querySelector(".axis.x .stroke");
    x_axis.setAttribute("y1", height / 2);
    x_axis.setAttribute("y2", height / 2);
    x_axis.setAttribute("x1", 0);
    x_axis.setAttribute("x2", width);

    /** render two a y axis after 1/3 and 2/3 of the grid width **/
    var y_axis_left = elem.querySelector(".axis.y .stroke.left");
    y_axis_left.setAttribute("x1", width * 1 / 3);
    y_axis_left.setAttribute("x2", width * 1 / 3);
    y_axis_left.setAttribute("y1", 0);
    y_axis_left.setAttribute("y2", height);

    var y_axis_right = elem.querySelector(".axis.y .stroke.right");
    y_axis_right.setAttribute("x1", width * 2 / 3);
    y_axis_right.setAttribute("x2", width * 2 / 3);
    y_axis_right.setAttribute("y1", 0);
    y_axis_right.setAttribute("y2", height);
  }

  var renderPath = function(series, height, width, path_elem) {
    if (!series.min) {
      series.min = 0.0;
    }

    if (!series.max) {
      series.max = Math.max.apply(null, series.values);
    }

    var padding_x = 0;
    var padding_y = 5;

    var points = scaleValues(series);

    var svg_line = [];
    for (var i = 0; i < points.length; ++i) {
      if (!isNaN(points[i].y)) {
        var dx = padding_x + (points[i].x * (width - padding_x * 2));
        var dy = padding_y + ((1.0 - points[i].y) * (height - padding_y * 2));
        svg_line.push(i == 0 ? "M" : "L", dx, dy);
      }
    }

    //var svg = elem.querySelector("svg");

    path_elem.setAttribute("d", svg_line.join(" "));
  };

  var getTemplate = function() {
    var tpl = document.createElement("svg");
    tpl.className = "sparkline";

    var p = document.createElement("path");
    tpl.appendChild(p);
    var c = document.createElement("circle");
    tpl.appendChild(c);

    return tpl;
  }

  getDimension = function(dimension, elem) {
    var value = elem.getAttribute(dimension);

    var idx = value.indexOf("%");
    if (idx > -1) {
      return elem.offsetWidth * (parseInt(value.substr(0, idx), 10) / 100);
    }

    return parseFloat(value);
  };

  var scaleValues = function(series) {
    var scaled = [];

    for (var i = 0; i < series.values.length; ++i) {
      var v = series.values[i];
      var x  = i / (series.values.length - 1);

      if (v < series.min) {
        scaled.push({x : x, y: 0});
        continue;
      }

      if (v > series.max) {
        scaled.push({x: x, y: 1.0});
        continue;
      }

      if (series.max - series.min == 0) {
        scaled.push({x: x, y: v});
      } else {
        scaled.push({x: x, y:(v - series.min) / (series.max - series.min)});
      }
    }

    return scaled;
  };

  return {
    render: render
  }

})();



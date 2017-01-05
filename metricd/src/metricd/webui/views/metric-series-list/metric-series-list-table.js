/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
if (typeof FnordMetric == undefined) {
  FnordMetric = {};
}

FnordMetric.SeriesTable = function(elem, series) {
  'use strict';

  var sparkline_renderer;

  var render = function() {
    var tbody = elem.querySelector("tbody");
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
    tbody.appendChild(tr);

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

      renderContextMenu(series.series_id, e);
      e.preventDefault();
    });


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
    tr.appendChild(td);
    renderSparkline(series, td);

  }

  var renderSparkline = function(series, td) {
    td.className = "sparkline";

    var sparkline_cfg = {
      series: [
        {
          values: series.values
        }
      ]
    };

    var html = sparkline_renderer.render(
        sparkline_cfg,
        td.offsetWidth,
        td.offsetHeight);

    td.innerHTML = html;
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

  /** init **/
  sparkline_renderer = new FnordMetric.SeriesTableSparklineRenderer();
  render();

  document.addEventListener("click", function(e) {
    hideContextMenu();
  }, false);
}

FnordMetric.SeriesTableSparklineRenderer = function() {
  'use strict';

  var padding = {
    top: 2,
    bottom: 2,
    left: 10,
    right: 10
  }

  /**
    * @param elem the html elem
    * @param cfg (Object) consists of multiple series object, each with 
    * values: an array of float values for the y axis
    * min (optinal): a float determining the min value
    * max(optinal): a float determining the max value
    **/
  this.render = function(cfg, width, height) {
    var grid_height = height - padding.top - padding.bottom;
    var grid_width = width - padding.left - padding.right;

    var html = ["<svg class='sparkline' style='height: ", grid_height, "px; width: ",
      grid_width, "px;'>"];


    html.push(renderGrid(grid_height, grid_width));

    html.push("<g class='lines'>");

    for (var i = 0; i < cfg.series.length; i++) {
      html.push(renderPath(cfg.series[i], grid_height, grid_width));
    }

    html.push("</g>");

    html.push("</svg>");

    return html.join("");
  }

  var renderGrid = function(height, width) {
    var html = [];
    html.push("<g class='axis x'>");

    /** render x axis in the middle of the sparkline **/
    html.push(
      "<line class='axis x stroke' y1='", height / 2, "' y2='", height / 2,
      "' x1='0' x2='", width, "'></line>");

    /** render two a y axis after 1/3 and 2/3 of the grid width **/
    html.push(
      "<line class='axis x stroke' y1='0' y2='", height,
      "' x1='", width * 1/3, "' x2='", width * 1/3, "'></line>");

    html.push(
      "<line class='axis x stroke' y1='0' y2='", height,
      "' x1='", width * 2/3, "' x2='", width * 2/3, "'></line>");

    html.push("</g>");

    return html.join("");
  }

  var renderPath = function(series, height, width) {
    if (!series.min) {
      series.min = 0.0;
    }

    if (!series.max) {
      series.max = Math.max.apply(null, series.values);
    }

    var padding_x = 0;
    var padding_y = 5;

    var points = scaleValues(series);

    var html = [];


    var svg_line = [];
    for (var i = 0; i < points.length; ++i) {
      if (!isNaN(points[i].y)) {
        var dx = padding_x + (points[i].x * (width - padding_x * 2));
        var dy = padding_y + ((1.0 - points[i].y) * (height - padding_y * 2));
        svg_line.push(i == 0 ? "M" : "L", dx, dy);
      }
    }


    html.push(
        "<path class='line' style='stroke:", series.color,
        ";' d='", svg_line.join(" "), "'></path>");

    return html.join("");
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

  var getDimension = function(dimension, elem) {
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

}


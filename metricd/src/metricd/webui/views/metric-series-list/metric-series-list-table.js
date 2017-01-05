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

FnordMetric.SeriesTable = function(elem) {
  'use strict';

  var columns = [
    {
      key: "series_id",
      title: "Series ID",
      sortable: true
    },
    {
      key: "value",
      title: "Value",
      sortable: true,
      colspan: "2"
    },
    {
      key: "context_menu",
      title: ""
    }
  ];

  var on_sort_callbacks = [];
  var sparkline_renderer;

  /**
   * Adds a callback that is called on a column sort event
   *
   * @param callback {function} a callback function
   */
  this.onSort = function(callback) {
    on_sort_callbacks.push(callback);
  }

  /**
   * Marks the column thats been sorted by
   *
   * @param col {string} the column's key
   * @param dir {string} the sort direction (must be either asc or desc)
   */
  this.setSort = function(col, dir) {
    columns.forEach(function(c) {
      if (c.key == col) {
        c.sorted = dir;
      } else {
        c.sorted = null;
      }
    });
  }

  /**
   * Renders a table for the provided series
   *
   * @param series {array} the request response
   */
  this.render = function(series) {
    if (!sparkline_renderer) {
      sparkline_renderer = new FnordMetric.SeriesTableSparklineRenderer();
    }

    var table = document.createElement("table");
    table.className = "fnordmetric-series-list-table";
    elem.appendChild(table);

    var thead = document.createElement("thead");
    table.appendChild(thead);
    renderHeader(thead);

    var tbody = document.createElement("tbody");
    table.appendChild(tbody);

    series.forEach(function(s) {
      /** skip summary row **/
      if (s.tags && s.tags.indexOf("summary") > -1) {
        return;
      }

      renderRow(s, tbody)
    });
  }

/********************************* private ***********************************/

  var renderHeader = function(thead) {
    var tr = document.createElement("tr");

    columns.forEach(function(col) {
      var th = document.createElement("th");
      th.className = col.key;
      th.innerHTML = col.title;

      if (col.colspan) {
        th.setAttribute("colspan", col.colspan);
      }

      if (col.sortable) {
        var sort_elem = document.createElement("span");
        sort_elem.classList.add("sort");
        th.appendChild(sort_elem);

        var sort_asc = document.createElement("i");
        sort_asc.classList.add("sort_asc");
        sort_elem.appendChild(sort_asc);

        var sort_desc = document.createElement("i");
        sort_desc.classList.add("sort_desc");
        sort_elem.appendChild(sort_desc);

        switch (col.sorted) {
          case "asc":
            sort_asc.classList.add("active");
            break;
          case "desc":
            sort_desc.classList.add("active");
            break;
          case null:
            break;
        }

        var sort_fn = function(dir) {
          on_sort_callbacks.forEach(function(f) {
            f(col, dir);
          });
        }

        sort_asc.addEventListener("click", function(e) {
          sort_fn("asc");
        }, false);

        sort_desc.addEventListener("click", function(e) {
          sort_fn("desc");
        }, false);
      }

      tr.appendChild(th);
    });

    thead.appendChild(tr);
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
      /** check if right click **/
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
    td.innerHTML = [
      "<div class='total'>",
        sum || "-",
      "</div>",
      "<div class='value_stats'>",
        value_stats_line,
      "</div>"
    ].join("");

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
    var menu = elem.querySelector(".fm-context_menu");

    if (!menu) {
      menu = document.createElement("div");
      menu.className = "fm-context_menu";

      menu.innerHTML = [
        "<ul>",
          "<li>Open in New Tab</li>",
          "<li>Open in New Tab</li>",
          "<li>Open in New Tab</li>",
        "</ul>",
      ].join("");

      elem.appendChild(menu);
    }

    menu.classList.add("active");

    var pos_x;
    var pointer_pos;

    /** right align contextmenu at the window's right edge **/
    if (window.innerWidth - ev.clientX < menu.offsetWidth) {
      pos_x = ev.clientX - menu.offsetWidth + 19;
      pointer_pos = "right";

    /** left align contextmenu at the window's left edge **/
    } else if (ev.clientX < menu.offsetWidth / 2) {
      pos_x = ev.clientX - 16;
      pointer_pos = "left";

    /** center contextmenu **/
    } else {
      pos_x = ev.clientX - menu.offsetWidth / 2;
      pointer_pos = "center";

    }

    menu.style.left = pos_x + "px";
    menu.setAttribute("data-pointer", pointer_pos);

    var pos_top = ev.clientY + window.scrollX + 5;
    menu.style.top = pos_top + "px";
  }

  var hideContextMenu = function() {
    var menu = elem.querySelector(".fm-context_menu");
    if (menu) {
      menu.classList.remove("active");
    }
  }

  /** init **/
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

    var html = [
      "<svg class='sparkline'",
        "style='height: ", grid_height, "px; width: ", grid_width, "px;'",
      ">"];


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
      "<line class='axis x stroke' ",
        "y1='", height / 2, "' y2='", height / 2, "' x1='0' x2='", width,
      "'></line>");

    /** render two a y axis after 1/3 and 2/3 of the grid width **/
    html.push(
      "<line class='axis x stroke' ",
        "y1='0' y2='", height, "' x1='", width * 1/3, "' x2='", width * 1/3,
      "'></line>");

    html.push(
      "<line class='axis x stroke' ",
        "y1='0' y2='", height, "' x1='", width * 2/3, "' x2='", width * 2/3,
      "'></line>");

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
    var padding_y = padding.top + padding.bottom;

    var points = scaleValues(series);

    var svg_line = [];
    for (var i = 0; i < points.length; ++i) {
      if (!isNaN(points[i].y)) {
        var dx = padding_x + (points[i].x * (width - padding_x * 2));
        var dy = padding_y + ((1.0 - points[i].y) * (height - padding_y * 2));
        svg_line.push(i == 0 ? "M" : "L", dx, dy);
      }
    }

    var html = [];
    html.push(
        "<path class='line' style='stroke:", series.color,
          ";' d='", svg_line.join(" "),
        "'></path>");

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


FnordMetric.views["fnordmetric.metric.serie.table"] = function(elem, params) {
  var table;

  var default_columns = [
    {
      key: "time",
      title: "Time",
      formatter: dateUtil.formatDateTime
    },
    {
      key: "value",
      title: "Value"
    }
  ];

  this.initialize = function() {
    var results = params.data;
    if (results.length > 0) {
      renderTable(results);
    } else {
      renderEmptyTable();
    }
  };

  var renderTable = function(values) {
    console.log(values);
    /* build columns and rows */
    var columns = default_columns;
    var rows = [];
    values.forEach(function(v) {
      var cells = {
        time: {
          value: v[0] / 1000
        },
        value: {
          value: v[1]
        }
      };

      rows.push({cells: cells});
    });

    /* initialize table */
    if (!table) {
      var tpl = templateUtil.getTemplate("fnordmetric-metric-serie-table-tpl");
      elem.appendChild(tpl);

      table = new fTable({columns: columns});
      /* navigate to id detail page */
      table.onClick(function(r) {
        params.app.navigateTo(params.route.args[0] + "/" + r.cells.sensor.value);
      });
    }

    table.setRows(rows);
    table.render(elem.querySelector(
        ".fnordmetric-metric-serie-table .metric-serie-table"));
    renderPagination(values.length);
  };

  var renderEmptyTable = function() {
    elem.innerHTML = "<div class='error_pane'>" +
        "<h2>We couldn't find anything</h2></div>";
  }


  //var renderValueColumnsDropdown = function(value_columns) {
  //  var dropdown = elem.querySelector(
  //      ".fnordmetric-metric-table z-dropdown.value_columns");
  //  var selector = new fMetricSelector(dropdown, value_columns);
  //  selector.render();

  //  dropdown.addEventListener("change", function(e) {
  //    var columns = this.getValue().split(",");
  //    value_columns.forEach(function(c) {
  //      var idx = columns.indexOf(c.key);
  //      if (c.hidden && idx > -1) {
  //        view_cfg.addColumn("value_columns", c.key);
  //      }

  //      if (!c.hidden && idx == -1) {
  //        view_cfg.removeColumn("value_columns", c.key);
  //      }
  //    });

  //    updatePath();
  //  });
  //}

  //var renderValueColumnsSidebar = function(value_columns) {
  //  var ul = elem.querySelector(
  //      ".fnordmetric-metric-table .sidebar .value_columns");
  //  value_columns.forEach(function(col) {
  //    var li = document.createElement("li");
  //    li.innerHTML = col.title;
  //    var icon = document.createElement("i");
  //    icon.className = "fa";
  //    li.className = col.hidden ? "" : "selected";

  //    li.appendChild(icon);
  //    ul.appendChild(li);

  //    icon.addEventListener("click", function(e) {
  //      if (col.hidden) {
  //        view_cfg.addColumn("value_columns", col.key);
  //      } else {
  //        view_cfg.removeColumn("value_columns", col.key);
  //      }
  //      updatePath();
  //    });

  //    if (!col.hidden) {
  //      li.addEventListener("mouseover", function(e) {
  //        table.highlightColumn(col.key);
  //      });

  //      li.addEventListener("mouseout", function(e) {
  //        table.unhiglightColumn();
  //      });
  //    }
  //  });
  //}

  var renderPagination = function(num_result_items) {
    var pager = elem.querySelector(".fnordmetric-metric-table .table_box z-pager");
    //pager.render(
    //    50,
    //    parseInt(params.view_cfg.getValue("offset"), 10),
    //    parseInt(params.view_cfg.getValue("limit"), 10));

    //pager.addEventListener("z-pager-turn", function(e) {
    //  params.view_cfg.updateValue("offset", this.getOffset());
    //  updatePath();
    //}, false);
  }
}

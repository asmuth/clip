FnordMetric.views["fnordmetric.metric.table"] = function(elem, params) {
  var table;

  this.initialize = function() {
    var page = zTemplateUtil.getTemplate("fnordmetric-metric-table-tpl");

    //zDomUtil.handleLinks(page, params.app.navigateTo);
    elem.appendChild(page);

    renderTable(params.data);
    renderPagination(params.data.rows.length);
  };

  var renderTable = function(data) {
    //initialize table
    if (!table) {
      table = new zTable({columns: params.view_cfg.getTableColumns()});

      /* navigate to id detail page */
      table.onClick(function(r) {
        params.app.navigateTo(params.route.args[0] + "/" + r.cells.sensor.value);
      });

      /* sort callback */
      table.onSort(function(column, direction) {
        params.view_cfg.updateValue("order", direction);
        params.view_cfg.updateValue("order_by", column.key);
        updatePath();
      });
    }

    table.setRows(fTableUtil.buildRows(data.columns, data.rows));
    table.render(elem.querySelector(".fnordmetric-metric-table .metric-table"));
  };


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
    pager.render(
        50,
        parseInt(params.view_cfg.getValue("offset"), 10),
        parseInt(params.view_cfg.getValue("limit"), 10));

    pager.addEventListener("z-pager-turn", function(e) {
      params.view_cfg.updateValue("offset", this.getOffset());
      updatePath();
    }, false);
  }
}

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

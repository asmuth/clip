/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Laura Schlimmer
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

if (FnordMetric === undefined) {
  FnordMetric = {};
}

if (FnordMetric.util === undefined) {
  FnordMetric.util = {};
}

FnordMetric.util.TableView = function(columns, elem) {
  var onRowClick = null;

  var table = document.createElement("table");
  table.className = "metric_list";
  var header = document.createElement("tr");
  header.className = "list_header";
  columns.map(function(column) {
    var header_cell = document.createElement("th");
    header_cell.innerHTML = column;
    header.appendChild(header_cell);
  });
  table.appendChild(header);
  elem.appendChild(table);

  function renderPagination(from, until, total) {
    var navbar = document.createElement("div");
    navbar.className = "pagination_navbar metric";
    var ttp_for = FnordMetric.createButton(
      "#", "pagination_tooltip", "&#8594;");
    var ttp_back = FnordMetric.createButton(
      "#", "pagination_tooltip",  "&#8592;");
    var label = document.createElement("div");
    label.className = "pagination_label";
    label.innerHTML = 
      from + " - " + until + " of " + total;
    navbar.appendChild(ttp_for);
    navbar.appendChild(ttp_back);
    navbar.appendChild(label);
    elem.insertBefore(navbar, table);
  }

  function updatePaginationLabel(from, until, total) {
    //FIXME
    var label = elem.querySelector(".pagination_label");
    label.innerHTML = 
      from + " - " + until + " of " + total;
  }



  function addRow(row) {
    var list_row = document.createElement("tr");
    //onRowClick = list_row.onclick;
    list_row.addEventListener('click', onRowClick, false);

    row.map(function(cell) {
      var list_cell = document.createElement("td");
      list_cell.innerHTML = cell;
      list_row.appendChild(list_cell);
    });
    table.appendChild(list_row);
  }

  return {
    "renderPagination" : renderPagination,
    "updatePaginationLabel" : updatePaginationLabel,
    "addRow": addRow,
    "onRowClick": onRowClick
  };
};


/*




  var renderResult = function(metrics_data, elem, isSearchResult, search_item) {
    var rows_per_side = 10;
    var pag_navbar;
    var list_container;
    var no_result_text = undefined;
    var end;

    function searchRows(search_key) {
      //destroyRows();
      destroyListPagination();
      //FIXME works but seems not to be the best solution
      var data = [];
      metrics_data.map(function(item) {
       if (item.key.indexOf(search_key) > -1) {
          data.push(item);
        }
      });
      renderTable(data);
    }

    var createListHeaderCells = function(labels) {
      labels.map(function(label) {
        var list_header_cell = document.createElement("th");
        list_header_cell.innerHTML = label;
        var createSortLink = function(symbol, order) {
          var sort_link = FnordMetric.createButton(
            "#", "caret", symbol);
          sort_link.setAttribute("id", label);
          list_header_cell.appendChild(sort_link);

          sort_link.addEventListener('click', function(e) {
            e.preventDefault();
            var sorted_data = sortColumns(metrics_data, this.id, order);
            renderTable(sorted_data);
          }, false);
        }
        createSortLink("&#x25B2;", "asc");
        createSortLink("&#x25BC;", "desc");
        list_header.appendChild(list_header_cell);
      });
    }

    var renderListPagination = function(metrics_data) {
      var start_index = 0;
      var end_index = rows_per_side;


      var pag_navbar = document.createElement("div");
      pag_navbar.className = "pagination_navbar metrics";

      var tooltipObj = {
        "for" :  {
          "arrow" : "&#8594;",
          "marginRight" : "0px"
        },
        "back" : {
          "arrow" : "&#8592;",
          "marginRight" : "2px"
        }
      }

      var createTooltip = function(type) {
        var tooltip = FnordMetric.createButton(
          "#", "pagination_tooltip", tooltipObj[type]["arrow"]);
        tooltip.style.marginRight = tooltipObj[type]["marginRight"];

        tooltip.addEventListener('click', function(e) {
          e.preventDefault();
          if (type == "for") {
            start_index = parseInt(this.id);
            end_index = Math.min(metrics_data.length, 
              start_index + rows_per_side);
          } else {
            start_index = Math.max(0, parseInt(this.id) - rows_per_side);
            end_index = start_index + rows_per_side;
          }
          tooltipEvent()
        }, false);

        return tooltip;
      }


      var pag_label = document.createElement("div");
      pag_label.className = "pagination_label";

      var updateLabel = function() {
        pag_label.innerHTML = "<b>" + (start_index +1) + 
          "</b><span> - </span><b>" + end_index + 
          "</b><span> of </span><b>" + metrics_data.length + "</b>";
      }

      var updateTooltips = function() {
        tooltip_for.setAttribute("id", end_index);
        tooltip_back.setAttribute("id" , start_index);

        tooltip_for.style.color = 
          (end_index == metrics_data.length) ? "#ddd" : "#444";

        tooltip_back.style.color = 
          (start_index == 0) ? "#ddd" : "#444";
      }

      var tooltipEvent = function() {
        updateLabel();
        updateTooltips();
        destroyRows();
        for (var i = start_index; i < end_index; i++) {
          createListItem(metrics_data[i]);
        }
      }


      var tooltip_for = createTooltip("for");
      var tooltip_back = createTooltip("back");

      updateLabel();
      updateTooltips();

      pag_navbar.appendChild(tooltip_for);
      pag_navbar.appendChild(tooltip_back);
      pag_navbar.appendChild(pag_label);
      if (list_container !== undefined) {
        elem.insertBefore(pag_navbar, list_container);
      } else {
        elem.appendChild(pag_navbar);
      }

      return pag_navbar;
    }

    var destroyListPagination = function() {
      if (pag_navbar === undefined) {
        return;
      }
      while (pag_navbar.firstChild) {
        pag_navbar.removeChild(pag_navbar.firstChild);
      }
        //FIXME
      elem.removeChild(document.querySelector(".pagination_navbar"));
    }

    var createListItem = function(data) {
      var list_item_row = document.createElement("tr");

      var i = 0;
      var list_elems = ["key", "labels", "insert", "total_bytes"];


      if (!data.converted) {
        parseLabels(data);
        convertTimestamp(data);
        data.converted = true;
      }

      list_elems.map(function(item) {
        var list_item = document.createElement("td");
        list_item.innerHTML = data[item];
        list_item_row.appendChild(list_item);
      });

      list_container.appendChild(list_item_row);

      list_item_row.addEventListener('click', function(e) {
        e.preventDefault();
              destroy(elem);
        FnordMetric.views.QueryPlayground().render(elem, query);
      }, false);
    }

    var renderTable = function(data) {
      if (data.length == 0) {
        no_result_text = renderNoResult();
        return;
      }
      if (list_container === undefined) {
        initTable();
      } else {
        destroyRows();
      }
      if (no_result_text !== undefined) {
        destroyNoResult();
      }
      if (data.length > rows_per_side) {
        renderListPagination(data);
        var end = rows_per_side;
      } else {
        var end = data.length;
      }
      for (var i = 0; i < end; i++) {
        createListItem(data[i]);
      }
    }

    var destroyRows = function() {
      if (list_container === undefined ||
          list_container === null) {
        return;
      }
      while (list_container.childNodes.length > 1) {
        list_container.removeChild(list_container.lastChild);
      }
    }

    var list_header;
    var initTable = function() {
      list_container = document.createElement("table");
      list_container.className = "metrics_list_container";

      list_header = document.createElement("tr");
      list_header.className = "metrics_list_header";
      createListHeaderCells(["Key", "Labels", "Last Insert", "Total stored bytes"]);
      list_container.appendChild(list_header);
      elem.appendChild(list_container);
    }

    var destroyTable = function() {
      if (list_container === undefined) {
        list_container = document.querySelector(
          ".metrics_list_container");
        if (list_container === null) {
          list_container = undefined;
          return;
        }
      }
      while (list_container.firstChild) {
        list_container.removeChild(list_container.firstChild);
      }
      console.log(elem);
      console.log(list_container.parentNode);
      (list_container.parentNode).removeChild(list_container);
      list_container = undefined;
    }

    var renderView = function() {
      destroyTable();
      destroyNoResult();

      if (isSearchResult) {
        searchRows(search_item);
      } else {
        if (metrics_data.length > rows_per_side) {
         pag_navbar = renderListPagination(metrics_data);
        }
        end = Math.min(metrics_data.length, rows_per_side);
        initTable();
        for (var i = 0; i < end;  i++) {
          createListItem(metrics_data[i], true);
        }
      }
    }

    renderView();
  }


*/

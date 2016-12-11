/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
fTable = function(table) {
  table.rows = table.rows || [];

  var on_sort = [];
  var on_click = [];
  var on_right_click = [];
  var on_control_click = [];

  this.getColumns = function() {
    return table.columns;
  }

  this.getColumn = function(column_name) {
    for (var i = 0; i < table.columns.length; ++i) {
      if (table.columns[i].key == column_name) {
        return table.columns[i];
      }
    }

    return null;
  }

  this.getRows = function() {
    return table.rows;
  }

  this.setRows = function(rows) {
    table.rows = rows;
  }

  this.addRow = function(row) {
    table.rows.push(row);
  }

  this.updateRows = function(key_column, updates) {
    for (var i = 0; i < table.rows.length; ++i) {
      var key_cell = table.rows[i].cells[key_column];
      if (key_cell && updates.hasOwnProperty(key_cell.value)) {
        var update = updates[key_cell.value];
        for (k in update) {
          table.rows[i].cells[k] = (table.rows[i].cells[k] || {});
          table.rows[i].cells[k].value = update[k];
        }
      }
    }
  }

  this.getColumnValues = function(column_names) {
    var result = [];
    for (var i = 0; i < column_names.length; ++i) {
      result.push([]);
    }

    table.rows.forEach(function(row) {
      for (var i = 0; i < column_names.length; ++i) {
        var cell = row.cells[column_names[i]];
        result[i].push(cell ? cell.value : null);
      }
    });

    return result;
  }

  this.highlightColumn = function(column_name) {
    this.unhiglightColumn();

    var cells = document.querySelectorAll("tr [data-key='" + column_name + "']");
    for (var i = 0; i < cells.length; i++) {
      cells[i].classList.add("highlight");
    }
  }

  this.unhiglightColumn = function() {
    var cells = document.querySelectorAll("tr .highlight");
    for (var i = 0; i < cells.length; i++) {
      cells[i].classList.remove("highlight");
    }
  }

  this.onClick = function(fn) {
    on_click.push(fn);
  }

  this.onRightClick = function(fn) {
    on_right_click.push(fn);
  }

  this.onControlClick = function(fn) {
    on_control_click.push(fn);
  }

  this.onSort = function(fn) {
    on_sort.push(fn);
  }

  this.sortByColumn = function(column_name, direction) {
    var comparator;
    if (table.columns) {
      for (var i = 0; i < table.columns.length; ++i) {
        if (table.columns[i].key == column_name) {
          table.columns[i].sorted = direction;
          comparator = table.columns[i].comparator;
        } else {
          delete table.columns[i].sorted;
        }
      }
    }

    if (!comparator) {
      comparator = function(a, b) {
        if (a < b) {
          return -1;
        }

        if (a > b) {
          return 1;
        }

        return 0;
      }
    }

    var reverse = direction == "desc";
    table.rows.sort(function(a, b) {
      var a_cell = a.cells[column_name];
      var a_value = a_cell ? a_cell.value : null;
      var b_cell = b.cells[column_name];
      var b_value = b_cell ? b_cell.value : null;

      if (reverse) {
        return comparator(b_value, a_value);
      } else {
        return comparator(a_value, b_value);
      }
    });
  }

  this.setVisibleColumns = function(visible_columns) {
    if (table.columns) {
      table.columns.forEach(function(col) {
        if (visible_columns.indexOf(col.key) < 0 && col.hideable != false) {
          col.hidden = true;
        } else {
          delete col.hidden;
        }
      });
    }
  };

  this.render = function(elem) {
    var thead = document.createElement("thead");
    var tbody = document.createElement("tbody");

    /* render columns header */
    var head_tr = document.createElement("tr");
    thead.appendChild(head_tr);

    table.columns.forEach(function(c) {
      if (c.hidden) {
        return;
      }

      var th = document.createElement("th");
      th.setAttribute("align", c.align || "left");
      th.setAttribute("data-key", c.key);
      th.innerHTML = c.title;

      if (c.sortable) {
        var sort_elem = document.createElement("span");
        sort_elem.classList.add("sort");
        th.appendChild(sort_elem);

        var sort_asc = document.createElement("i");
        sort_asc.classList.add("sort_asc");
        sort_elem.appendChild(sort_asc);

        var sort_desc = document.createElement("i");
        sort_desc.classList.add("sort_desc");
        sort_elem.appendChild(sort_desc);

        switch (c.sorted) {
          case "asc":
            sort_asc.classList.add("active");
            break;
          case "desc":
            sort_desc.classList.add("active");
            break;
        }

        var sort_fn = function(dir) {
          on_sort.forEach(function(f) {
            f(c, dir);
          });
        };

        DomUtil.onClick(sort_asc, function() {
          sort_fn("asc");
        });

        DomUtil.onClick(sort_desc, function() {
          sort_fn("desc");
        });
      }

      head_tr.appendChild(th)
    });

    /* render rows */
    table.rows.forEach(function(row) {
      var tr = document.createElement("tr");

      table.columns.forEach(function(col) {
        if (col.hidden) {
          return;
        }

        var cell = {};
        if (row.cells.hasOwnProperty(col.key)) {
          cell = row.cells[col.key];
        }

        var td = document.createElement("td");
        td.setAttribute("align", cell.align || col.align || "left");
        td.setAttribute("data-key", col.key);
        if (cell.class) {
          td.setAttribute("class", cell.class);
        }

        var td_a = document.createElement("a");
        if (cell.href) {
          td_a.setAttribute("href", cell.href);
        }

        var cell_value = cell.value;
        if (cell.value_html) {
          cell_value = cell.value_html;
        } else {
          if (cell_value && col.formatter) {
            cell_value = col.formatter(cell_value);
          }

          if (!cell_value) {
            cell_value = table.empty_cell_value || "—";
          }

          cell_value = DomUtil.escapeHTML(cell_value);
        }

        if (col.is_control) {
          DomUtil.onClick(td_a, function() {
            on_control_click.forEach(function(fn) {
                fn.apply(td_a, [row, col.key]);
            });
          });
        }

        td_a.innerHTML = cell_value;
        td.appendChild(td_a)
        tr.appendChild(td)
      });

      tr.addEventListener("click", function(e) {
        on_click.forEach(function(f) {
          f(row);
        });
        return false;
      });

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

        on_right_click.forEach(function(f) {
          f(e, row);
        });
        return false;
      });

      tbody.appendChild(tr)
    });

    /* commit changes to DOM */
    elem.innerHTML = "";
    elem.appendChild(thead);
    elem.appendChild(tbody);
  };
}


FnordMetric.MetricTableFilter = function(elem) {
  var columns;

  this.render = function(visible_columns) {
    columns = visible_columns;

    var inner = elem.querySelector(".inner");
    zDomUtil.clearChildren(inner);

    var tpl = zTemplateUtil.getTemplate(
        "fnordmetric-metric-table-filter-modal-tpl");

    tpl.querySelector("z-dropdown.columns").addEventListener("change", function(e) {
      renderFilterSelection(this.getValue());
    }, false);

    tpl.querySelector("button.close").addEventListener("click", function() {
      elem.close();
    }, false);

    inner.appendChild(tpl);

    renderColumnSelection();

    elem.show();
  }

  var renderColumnSelection = function() {
    var columns_dropdown = elem.querySelector("z-dropdown.columns");
    var items = [];
    columns.forEach(function(c) {
      if (c.hidden) {
        return true;
      }

      var item = document.createElement("z-dropdown-item");
      item.setAttribute("data-value", c.key);
      item.innerHTML = c.title;
      items.push(item);
    });

    columns_dropdown.setDropdownItems(items);
  }

  var renderFilterSelection = function(column) {
    console.log("render filter selection");
    var section = elem.querySelector("section.filter");
    section.classList.remove("disabled");
  }
}

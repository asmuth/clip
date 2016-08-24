fMetricSelector = function(elem, metrics) {
  var on_change = [];

  this.onChange = function(f) {
    on_change.push(f);
  };

  this.render = function() {
    elem.setAttribute("data-multiselect", "true");
    var flyout = document.createElement("z-dropdown-flyout");
    flyout.innerHTML = "<z-dropdown-items></z-dropdown-items>";
    elem.appendChild(flyout);

    var header = elem.querySelector("z-dropdown-header-value");
    header.setAttribute("data-static", "true");
    elem.init();

    var items = [];
    var selected_metrics = [];

    metrics.forEach(function(m) {
      //metrics are per default hideable unless hideable = false is set
      if (m.hideable == false) {
        return;
      }

      var item = document.createElement("z-dropdown-item");
      item.setAttribute("data-value", m.key);
      item.innerHTML = "<z-checkbox></z-checkbox> " + m.title;
      items.push(item);

      if (!m.hidden) {
        selected_metrics.push(m.key);
      }
    });

    elem.setDropdownItems(items);
    elem.setValue(selected_metrics);
    renderHeader(header, selected_metrics.length);

    elem.addEventListener("change", function(e) {
      var val = elem.getValue().split(",");
      renderHeader(header, val.length)
    });
  };

  var renderHeader = function(elem, num_selected_metrics) {
    elem.innerHTML = num_selected_metrics + "/" + metrics.length + " Metrics";
  };
};

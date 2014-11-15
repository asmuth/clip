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

if (FnordMetric.views === undefined) {
  FnordMetric.views = {};
}

FnordMetric.util.Autocomplete = function(elem, input, callback) {
  var source = [];
  var position;
  var list = document.createElement("ul");
  var items;
  var list_items = [];
  var current_value;
  var hover = false;
  list.className = "autocomplete";

  function renderListItems() {
    list_items = [];
    items.map(function(item) {
      var li = document.createElement("li");
      li.innerHTML = "<a href='#'>"+item+"</href>";
      list.appendChild(li);
      list_items.push(li);

      li.addEventListener('click', function(e) {
        e.stopPropagation();
        input.value = this.innerText;
        resetList();
        callback(this.innerText);
      }, false);

      li.addEventListener('mouseover', function() {
        hover = true;
        if (list_items[position] != undefined) {
          list_items[position].className = "";
        }
      }, false);

      li.addEventListener('mouseout', function() {
        hover = false;
      }, false);
    });
  }

  function keyNavigation(list_items, i) {
    if (i > 0 && i < list_items.length) {
      if (i > 0) {
        list_items[i - 1].className = "";
      }
      if (i+1 < list_items.length) {
        list_items[i+1].className = "";
      }
      if (!hover) {
        list_items[i].className = "hover";
      }
    }
  }

  function resetList() {
    list.innerHTML = "";
    position = -1;
    current_value = "";
  }

  function init() {

    input.addEventListener('input', function() {
      if (input.value.length == 0) {
        resetList();
        if (list.parentNode == elem) {
          elem.removeChild(list);
        }
        return;
      }
      items = FnordMetric.util.filterStringArray(
        source, input.value, 10);
      resetList();
      renderListItems();
      elem.appendChild(list);
    }, false);

    document.addEventListener('click', function() {
      resetList();
    }, false);


    input.addEventListener('keydown', function(e) {
      switch (e.keyCode) {
        /* Enter */
        case 13:
          e.preventDefault();
          if (current_value.length > 0){
            input.value = current_value;
          }
          resetList();
          callback(input.value);
          break;
        /* Up arrow */
        case 38:
          position--;
          keyNavigation(
            list_items, position);
          current_value = items[position];
          break;
        /* Down arrow */
        case 40:
          position++;
          keyNavigation(
            list_items, position);
          current_value = items[position];
          break;
        default:
          break;
      }
    }, false);
  }

  FnordMetric.httpGet("/metrics", function(r) {
    if (r.status == 200) {
      var json = JSON.parse(r.response);
      json.metrics.map(function(metric) {
        source.push(metric.key);
      });
      items = source;
      init();
    }
  });


}


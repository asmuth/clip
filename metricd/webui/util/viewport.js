/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
var Viewport = function(elem) {
  var current_view;

  this.setView = function(view, params) {
    if (current_view && current_view.destroy) {
      current_view.destroy();
    }

    
    elem.innerHTML = "";
    current_view = {};

    view.call(current_view, elem, params);

    if (current_view.initialize) {
      current_view.initialize.call(current_view);
    }
  }

  this.getView = function() {
    return current_view;
  }

}

/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

FM = this.FM || {};

FM.FilterComponent = function(elem) {
  var submit_callbacks = [];

  this.init = function(filter_value) {
    if (filter_value) {
      elem.value = DOMUtil.escapeHTML(filter_value);
    }

    var this_;
    DOMUtil.onEnter(elem, function() {
      submit_callbacks.forEach(function(fn) {
        fn(elem.value);
      });
    });
  }

  this.onSubmit = function(callback_fn) {
    submit_callbacks.push(callback_fn);
  }
}


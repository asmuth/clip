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
Fnord = {
  components: {}
};

Fnord.getTemplate = function(component, template) {
  var template_selector = "#" + component + "-" + template + "-tpl";

  var template = document.querySelector(template_selector);
  if (!template) {
    var template_import = document.querySelector(
        "link[data-component='" + component + "']");

    if (!template_import) {
      return null;
    }

    template = template_import.import.querySelector(template_selector);
  }

  if (!template) {
    return null;
  }

  return document.importNode(template.content, true);
}

Fnord.registerComponent = function(component, cb) {
  var proto = Object.create(HTMLElement.prototype);
  cb.apply(proto);
  Fnord.components[component] = proto;
  document.registerElement(component, { prototype: proto });
  return proto;
}

Fnord.ready = function() {
  window.dispatchEvent(new Event("fn-ready"));
};

Fnord.ready();

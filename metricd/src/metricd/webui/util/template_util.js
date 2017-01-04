/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
templateUtil = this.templateUtil || (function() {
  var enable_html5_import = false; // google only technology, not even properly documented :(
  var enable_html5_templates = ("content" in document.createElement("template"));
  var enable_html5_importnode = 'importNode' in document;

  try {
    document.importNode(document.createElement('div'));
  } catch (e) {
    enable_html5_importnode = false;
  }

  function importNodeFallback(node, deep) {
    var a, i, il, doc = document;

    switch (node.nodeType) {

      case document.DOCUMENT_FRAGMENT_NODE:
        var new_node = document.createDocumentFragment();
        while (child = node.firstChild) {
          new_node.appendChild(node);
        }
        return new_node;

      case document.ELEMENT_NODE:
        var new_node = doc.createElementNS(node.namespaceURI, node.nodeName);
        if (node.attributes && node.attributes.length > 0) {
          for (i = 0, il = node.attributes.length; i < il; i++) {
            a = node.attributes[i];
            try {
              new_node.setAttributeNS(
                  a.namespaceURI,
                  a.nodeName,
                  node.getAttribute(a.nodeName));
            } catch (err) {}
          }
        }
        if (deep && node.childNodes && node.childNodes.length > 0) {
          for (i = 0, il = node.childNodes.length; i < il; i++) {
            new_node.appendChild(
                importNodeFallback(node.childNodes[i],
                deep));
          }
        }
        return new_node;

      case document.TEXT_NODE:
      case document.CDATA_SECTION_NODE:
      case document.COMMENT_NODE:
        return doc.createTextNode(node.nodeValue);

    }
  }

  var getTemplate = function(template_id) {
    var template_selector = "#" + template_id;

    var template = document.querySelector(template_selector);

    if (!template && enable_html5_import) {
      var template_imports = document.querySelectorAll("link[rel='import']");

      for (var i = 0; !template && i < template_imports.length; ++i) {
        template = template_imports[i].import.querySelector(template_selector);
      }
    }

    if (!template) {
      return null;
    }

    var content;
    if (enable_html5_templates) {
      content = template.content;
    } else {
      content = document.createDocumentFragment();
      var children = template.children;

      for (var j = 0; j < children.length; j++) {
        content.appendChild(children[j].cloneNode(true));
      }
    }

    if (enable_html5_importnode) {
      return document.importNode(content, true);
    } else {
      return importNodeFallback(content, true);
    }
  };

  return {
    getTemplate: getTemplate
  };
})();


/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
fEmbedPopup = function(elem, query) {
  var popup;
  var inner_window;

  function close() {
    if (popup.parentNode == elem) {
      elem.removeChild(popup);
    }
  }

  function render() {
    var tpl = templateUtil.getTemplate("fnordmetric-embed-popup");
    popup = tpl.querySelector("f-popup");

    tpl.querySelector("button.close").addEventListener("click", close, false);

    elem.appendChild(tpl);
    popup.show();
  }

  function embedIFrame() {
    //code.innerHTML = '&lt;iframe<br />&nbsp;&nbsp;&nbsp;&nbsp;width="800"<br />&nbsp;&nbsp;&nbsp;&nbsp;height="400"<br />&nbsp;&nbsp;&nbsp;&nbsp;frameBorder="0"<br />&nbsp;&nbsp;&nbsp;&nbsp;src="' + queryUrl() + '"&gt;<br />&nbsp;&nbsp;&nbsp;&nbsp;&lt;/iframe&gt;';
  }

  function embedURL() {
    inner_window.innerHTML = "Embed URL:";
    var code = document.createElement("code");
    code.innerHTML = queryUrl();
    inner_window.appendChild(code);
  }

  function embedHTML() {
    var code1 = document.createElement("code");
   // code1.innerHTML = '&lt;script href="http://' + document.location.host + '/s/fnordmetric.js" type="text/javascript"&gt;&lt;script&gt;';
    var code2 = document.createElement("code");
   // code2.innerHTML = '&lt;fm-chart&gt;<br />' + query + '<br />&lt;/fm-chart&gt;';
  }

  function queryUrl() {
    return "http://" + document.location.host + "/query?width=800&height=400&format=svg&q=" + encodeURIComponent(query);
  }

  return {
    "render" : render,
  }
}

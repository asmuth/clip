/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

fmLoader = this.fmLoader || {};

fmLoader.load = function(url, callback) {
  window.setTimeout(function() {
    var http = new XMLHttpRequest();
    http.open("GET", url, true);
    http.send();

    var base = this;
    http.onreadystatechange = function() {
      if (http.readyState == 4) {
        if (http.status == 200) {
          var dummy = document.createElement("div");
          dummy.innerHTML = http.responseText;
          dummy.style.display = "none";
          document.body.appendChild(dummy);

          var scripts = dummy.getElementsByTagName('script');
          for (var i = 0; i < scripts.length; i++) {
            var script = document.createElement('script');
            script.type = scripts[i].type;
            if (scripts[i].src) {
              script.src = scripts[i].src;
            } else {
              script.innerHTML = scripts[i].innerHTML;
            }

            document.head.appendChild(script);
            callback(true);
          }
        } else {
          callback(false);
        }
      }
    }
  }, 0);
}

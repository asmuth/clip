if (typeof FnordMetric == "undefined") {
  FnordMetric = {};
}

FnordMetric.WebUI = function() {
  var setupLayoutHTML = function() {
    var navbar = document.createElement("div");
    navbar.id = "navbar";
    document.body.appendChild(navbar);
  }


  setupLayoutHTML();
}

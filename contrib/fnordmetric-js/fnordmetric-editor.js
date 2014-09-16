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
if (typeof FnordMetric == "undefined") {
  FnordMetric = {};
}

FnordMetric.Editor = {
  tab_width: 4,
  font: "14px monospace",
  patterns: {
    "#d33682": /(SELECT|FROM|WHERE|GROUP|ORDER|BY|HAVING|LIMIT|OFFSET|ASC|DESC|COMMA|DOT|IDENTIFIER|STRING|NUMERIC|SEMICOLON|LPAREN|RPAREN|AND|OR|EQUAL|PLUS|MINUS|ASTERISK|SLASH|NOT|TRUE|FALSE|BANG|CIRCUMFLEX|TILDE|PERCENT|DIV|MOD|AMPERSAND|PIPE|LSHIFT|RSHIFT|LT|GT|BEGIN|CREATE|WITH|IMPORT|TABLE|ON|OFF|DRAW|LINECHART|AREACHART|BARCHART|POINTCHART|HEATMAP|HISTOGRAM|AXIS|TOP|RIGHT|BOTTOM|LEFT|ORIENTATION|HORIZONTAL|VERTICAL|STACKED|XDOMAIN|YDOMAIN|ZDOMAIN|XGRID|YGRID|LOGARITHMIC|INVERT|TITLE|SUBTITLE|GRID|LABELS|TICKS|INSIDE|OUTSIDE|ROTATE|LEGEND)\s/,
    "#6c71c4": /AS\s/,
    "#2aa198": /\b([-+]?)(\d+(.\d+)?)((?=;)?)\b/,
    "#fff": /\s+/,
    "#002b36": /\S+/
  }
};

FnordMetric.Editor.extendTextarea = function(elem) {
  elem.value = "IMPORT TABLE example_data\n" +
      "    FROM 'csv:measurement.csv?headers=true'\n\n" +
      "    DRAW LINECHART WITH\n" +
      "        AXIS BOTTOM\n" +
      "        AXIS LEFT;\n\n" +
      "    SELECT 'data' AS series, FROM_TIMESTAMP(time) AS x, value2 * 1000 AS y\n" +
      "        FROM example_data\n" +
      "        WHERE series = 'measurement2';\n";

  var parser = new FnordMetric.Editor.Parser(FnordMetric.Editor.patterns, true);
  var decorator = new FnordMetric.Editor.TextareaDecorator(elem, parser);
}

/**
 * This method is based on (basically, copied from) Colin Kuebler's LDT
 *    https://github.com/kueblc/LDT -- Dual licensed under GPLv3 and MIT
 *
 * Parser - Generates a tokenizer from regular expressions for TextareaDecorator
 */
FnordMetric.Editor.Parser = function(rules, i) {
  var api = this;
  var i = i ? 'i' : '';
  var parseRE = null;
  var ruleSrc = [];
  var ruleMap = {};

  api.add = function(rules) {
    for (var rule in rules) {
      var s = rules[rule].source;
      ruleSrc.push(s);
      ruleMap[rule] = new RegExp('^('+s+')$', i);
    }
    parseRE = new RegExp(ruleSrc.join('|'), 'g'+i);
  };

  api.tokenize = function(input){
    return input.match(parseRE);
  };

  api.identify = function(token){
    for( var rule in ruleMap ){
      if( ruleMap[rule].test(token) ){
        return rule;
      }
    }
  };

  api.add( rules );

  return api;
};

/**
 * This method is based on (basically, copied from) Colin Kuebler's LDT
 *    https://github.com/kueblc/LDT -- Dual licensed under GPLv3 and MIT
 *
 * TextareaDecorator - Builds and maintains a styled output layer under a
 * textarea input layer
 */
FnordMetric.Editor.TextareaDecorator = function(textarea, parser) {
  var api = this;
  var font_str = FnordMetric.Editor.font;
  var padding_str = "20px";

  var parent = document.createElement("div");
  parent.style.padding = padding_str;
  parent.style.font = font_str;
  parent.style.overflow = "auto";
  parent.style.position = "relative";

  var output = document.createElement("pre");
  output.style.margin = "0";
  output.style.MozPaddingStart = "1px";
  output.style.font = font_str;
  output.style.minHeight = "300px";
  parent.appendChild(output);

  var label = document.createElement("label");
  label.style.padding = padding_str;
  label.style.position = "absolute";
  label.style.top = "0";
  label.style.left = "0";
  label.style.width = "100%";
  label.style.height = "100%";
  label.style.display = "inline";
  label.style.boxSizing = "border-box";
  label.style.cursor = "text";
  parent.appendChild(label);

  textarea.parentNode.replaceChild(parent, textarea);
  textarea.className = '';
  textarea.spellcheck = false;
  textarea.wrap = "off";
  textarea.style.font = font_str;
  textarea.style.margin = "0";
  textarea.style.padding = "0";
  textarea.style.border = "0";
  textarea.style.background = "0";
  textarea.style.outline = "none";
  textarea.style.resize = "none";
  textarea.style.overflow = "hidden";
  textarea.style.maxWidth = "100%";
  textarea.style.minHeight = "100%";
  textarea.style.filter = "alpha(opacity = 20)";
  textarea.style.color = "rgba(0,0,0,.1)";
  label.appendChild(textarea);

  textarea.addEventListener(
      'keydown',
      function (e) {
        if(e.keyCode === 9) {
          var start = this.selectionStart;
          var end = this.selectionEnd;
          var target = e.target;
          var value = target.value;
          var tab_str = "";
          for (var i = 0; i < FnordMetric.Editor.tab_width; i++) {
            tab_str += " ";
          }

          target.value =
              value.substring(0, start) + tab_str + value.substring(end);

          this.selectionStart = this.selectionEnd = start +
            FnordMetric.Editor.tab_width;

          e.preventDefault();
          api.update();
        } 
      },
      false);


  var color = function(input, output, parser) {
    var oldTokens = output.childNodes;
    var newTokens = parser.tokenize(input);

    /*while (output.firstChild) {
      output.removeChild(output.firstChild);
    }

    for (var i = 0; i < newTokens.length; i++) {
      var span = document.createElement("span");
      span.style.color = parser.identify(newTokens[i]);
      span.textContent = span.innerText = newTokens[i];
      output.insertBefore(span, null);
    }*/

    var firstDiff, lastDiffNew, lastDiffOld;
    // find the first difference
    for (firstDiff = 0; firstDiff < newTokens.length && firstDiff < oldTokens.length; firstDiff++) {
      if (newTokens[firstDiff] !== oldTokens[firstDiff].textContent) {break;}
    }
    // trim the length of output nodes to the size of the input
    while (newTokens.length < oldTokens.length) {
      output.removeChild(oldTokens[firstDiff]);

    }
      
    // find the last difference
    for( lastDiffNew = newTokens.length-1, lastDiffOld = oldTokens.length-1; firstDiff < lastDiffOld; lastDiffNew--, lastDiffOld-- )
      if( newTokens[lastDiffNew] !== oldTokens[lastDiffOld].textContent ) break;
    // update modified spans
    for( ; firstDiff <= lastDiffOld; firstDiff++ ){
      oldTokens[firstDiff].style.color = parser.identify(newTokens[firstDiff]);
      oldTokens[firstDiff].textContent = oldTokens[firstDiff].innerText = newTokens[firstDiff];
    }
    // add in modified spans
    for( var insertionPt = oldTokens[firstDiff] || null; firstDiff <= lastDiffNew; firstDiff++ ){
      var span = document.createElement("span");
      span.style.color = parser.identify(newTokens[firstDiff]);
      span.textContent = span.innerText = newTokens[firstDiff];
      output.insertBefore( span, insertionPt );
    }
  };

  api.input = textarea;
  api.output = output;
  api.update = function(){
    var input = textarea.value;
    if (input) {
      color(input, output, parser);
      // determine the best size for the textarea
      var lines = input.split('\n');
      // find the number of columns
      var maxlen = 0;
      var curlen;
      for (var i = 0; i < lines.length; i++ ){
        var curlen = lines[i].length;
        // store the greatest line length thus far
        maxlen = maxlen > curlen ? maxlen : curlen;
      }
      textarea.cols = maxlen + 1;
      textarea.rows = lines.length + 1;
    } else {
      // clear the display
      output.innerHTML = '';
      // reset textarea rows/cols
      textarea.cols = textarea.rows = 1;
    }
  };

  // detect all changes to the textarea,
  // including keyboard input, cut/copy/paste, drag & drop, etc
  if( textarea.addEventListener ){
    // standards browsers: oninput event
    textarea.addEventListener( "input", api.update, false );
  } else {
    // MSIE: detect changes to the 'value' property
    textarea.attachEvent( "onpropertychange",
      function(e){
        if( e.propertyName.toLowerCase() === 'value' ){
          api.update();
        }
      }
    );
  }

  api.update();
  textarea.focus();
  return api;
};

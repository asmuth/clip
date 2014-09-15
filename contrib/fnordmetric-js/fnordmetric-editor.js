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

FnordMetric.Editor = {};

FnordMetric.Editor.extendTextarea = function(elem) {
  var parser = new FnordMetric.Editor.Parser({
        whitespace: /\s+/,
        red: /red/,
        orange: /orange/,
        yellow: /yellow/,
        green: /green/,
        blue: /blue/,
        indigo: /indigo/,
        violet: /violet/,
        other: /\S+/ },
        true);

  var decorator = new FnordMetric.Editor.TextareaDecorator(elem, parser);
}

/**
 * This method is based on (basically, copied from) Colin Kuebler's LDT
 *    https://github.com/kueblc/LDT -- Dual licensed under GPLv3 and MIT
 *
 * Parser - Generates a tokenizer from regular expressions for TextareaDecorator
 */
FnordMetric.Editor.Parser = function(rules, i) {
  /* INIT */
  var api = this;

  // variables used internally
  var i = i ? 'i' : '';
  var parseRE = null;
  var ruleSrc = [];
  var ruleMap = {};

  api.add = function( rules ){
    for( var rule in rules ){
      var s = rules[rule].source;
      ruleSrc.push( s );
      ruleMap[rule] = new RegExp('^('+s+')$', i );
    }
    parseRE = new RegExp( ruleSrc.join('|'), 'g'+i );
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
FnordMetric.Editor.TextareaDecorator = function( textarea, parser ) {
  var api = this;
  var font_str = "14px monospace";
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
  textarea.filter = "alpha(opacity = 20)";
  textarea.color = "color: rgba(0,0,0,.01)";
  label.appendChild(textarea);


  var color = function( input, output, parser ){
    var oldTokens = output.childNodes;
    var newTokens = parser.tokenize(input);
    var firstDiff, lastDiffNew, lastDiffOld;
    // find the first difference
    for( firstDiff = 0; firstDiff < newTokens.length && firstDiff < oldTokens.length; firstDiff++ )
      if( newTokens[firstDiff] !== oldTokens[firstDiff].textContent ) break;
    // trim the length of output nodes to the size of the input
    while( newTokens.length < oldTokens.length )
      output.removeChild(oldTokens[firstDiff]);
    // find the last difference
    for( lastDiffNew = newTokens.length-1, lastDiffOld = oldTokens.length-1; firstDiff < lastDiffOld; lastDiffNew--, lastDiffOld-- )
      if( newTokens[lastDiffNew] !== oldTokens[lastDiffOld].textContent ) break;
    // update modified spans
    for( ; firstDiff <= lastDiffOld; firstDiff++ ){
      oldTokens[firstDiff].className = parser.identify(newTokens[firstDiff]);
      oldTokens[firstDiff].textContent = oldTokens[firstDiff].innerText = newTokens[firstDiff];
    }
    // add in modified spans
    for( var insertionPt = oldTokens[firstDiff] || null; firstDiff <= lastDiffNew; firstDiff++ ){
      var span = document.createElement("span");
      span.className = parser.identify(newTokens[firstDiff]);
      span.textContent = span.innerText = newTokens[firstDiff];
      output.insertBefore( span, insertionPt );
    }
  };

  api.input = textarea;
  api.output = output;
  api.update = function(){
    var input = textarea.value;
    if( input ){
      color( input, output, parser );
      // determine the best size for the textarea
      var lines = input.split('\n');
      // find the number of columns
      var maxlen = 0, curlen;
      for( var i = 0; i < lines.length; i++ ){
        // calculate the width of each tab
        var tabLength = 0, offset = -1;
        while( (offset = lines[i].indexOf( '\t', offset+1 )) > -1 ){
          tabLength += 7 - (tabLength + offset) % 8;
        }
        var curlen = lines[i].length + tabLength;
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
  // initial highlighting
  api.update();

  return api;
};

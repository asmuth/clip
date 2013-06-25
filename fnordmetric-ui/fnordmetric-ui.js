/* this is an automatically generated file, don't modify it... */
(function(){if (!Date.now) Date.now = function() {
  return +new Date;
};
try {
  document.createElement("div").style.setProperty("opacity", 0, "");
} catch (error) {
  var fnord3_style_prototype = CSSStyleDeclaration.prototype,
      fnord3_style_setProperty = fnord3_style_prototype.setProperty;
  fnord3_style_prototype.setProperty = function(name, value, priority) {
    fnord3_style_setProperty.call(this, name, value + "", priority);
  };
}
fnord3 = {version: "2.9.1"}; // semver
function fnord3_class(ctor, properties) {
  try {
    for (var key in properties) {
      Object.defineProperty(ctor.prototype, key, {
        value: properties[key],
        enumerable: false
      });
    }
  } catch (e) {
    ctor.prototype = properties;
  }
}
var fnord3_array = fnord3_arraySlice; // conversion for NodeLists

function fnord3_arrayCopy(pseudoarray) {
  var i = -1, n = pseudoarray.length, array = [];
  while (++i < n) array.push(pseudoarray[i]);
  return array;
}

function fnord3_arraySlice(pseudoarray) {
  return Array.prototype.slice.call(pseudoarray);
}

try {
  fnord3_array(document.documentElement.childNodes)[0].nodeType;
} catch(e) {
  fnord3_array = fnord3_arrayCopy;
}

var fnord3_arraySubclass = [].__proto__?

// Until ECMAScript supports array subclassing, prototype injection works well.
function(array, prototype) {
  array.__proto__ = prototype;
}:

// And if your browser doesn't support __proto__, we'll use direct extension.
function(array, prototype) {
  for (var property in prototype) array[property] = prototype[property];
};
fnord3.map = function(object) {
  var map = new fnord3_Map;
  for (var key in object) map.set(key, object[key]);
  return map;
};

function fnord3_Map() {}

fnord3_class(fnord3_Map, {
  has: function(key) {
    return fnord3_map_prefix + key in this;
  },
  get: function(key) {
    return this[fnord3_map_prefix + key];
  },
  set: function(key, value) {
    return this[fnord3_map_prefix + key] = value;
  },
  remove: function(key) {
    key = fnord3_map_prefix + key;
    return key in this && delete this[key];
  },
  keys: function() {
    var keys = [];
    this.forEach(function(key) { keys.push(key); });
    return keys;
  },
  values: function() {
    var values = [];
    this.forEach(function(key, value) { values.push(value); });
    return values;
  },
  entries: function() {
    var entries = [];
    this.forEach(function(key, value) { entries.push({key: key, value: value}); });
    return entries;
  },
  forEach: function(f) {
    for (var key in this) {
      if (key.charCodeAt(0) === fnord3_map_prefixCode) {
        f.call(this, key.substring(1), this[key]);
      }
    }
  }
});

var fnord3_map_prefix = "\0", // prevent collision with built-ins
    fnord3_map_prefixCode = fnord3_map_prefix.charCodeAt(0);
function fnord3_identity(d) {
  return d;
}
function fnord3_this() {
  return this;
}
function fnord3_true() {
  return true;
}
function fnord3_functor(v) {
  return typeof v === "function" ? v : function() { return v; };
}

fnord3.functor = fnord3_functor;
// Copies a variable number of methods from source to target.
fnord3.rebind = function(target, source) {
  var i = 1, n = arguments.length, method;
  while (++i < n) target[method = arguments[i]] = fnord3_rebind(target, source, source[method]);
  return target;
};

// Method is assumed to be a standard D3 getter-setter:
// If passed with no arguments, gets the value.
// If passed with arguments, sets the value and returns the target.
function fnord3_rebind(target, source, method) {
  return function() {
    var value = method.apply(source, arguments);
    return arguments.length ? target : value;
  };
}
fnord3.ascending = function(a, b) {
  return a < b ? -1 : a > b ? 1 : a >= b ? 0 : NaN;
};
fnord3.descending = function(a, b) {
  return b < a ? -1 : b > a ? 1 : b >= a ? 0 : NaN;
};
fnord3.mean = function(array, f) {
  var n = array.length,
      a,
      m = 0,
      i = -1,
      j = 0;
  if (arguments.length === 1) {
    while (++i < n) if (fnord3_number(a = array[i])) m += (a - m) / ++j;
  } else {
    while (++i < n) if (fnord3_number(a = f.call(array, array[i], i))) m += (a - m) / ++j;
  }
  return j ? m : undefined;
};
fnord3.median = function(array, f) {
  if (arguments.length > 1) array = array.map(f);
  array = array.filter(fnord3_number);
  return array.length ? fnord3.quantile(array.sort(fnord3.ascending), .5) : undefined;
};
fnord3.min = function(array, f) {
  var i = -1,
      n = array.length,
      a,
      b;
  if (arguments.length === 1) {
    while (++i < n && ((a = array[i]) == null || a != a)) a = undefined;
    while (++i < n) if ((b = array[i]) != null && a > b) a = b;
  } else {
    while (++i < n && ((a = f.call(array, array[i], i)) == null || a != a)) a = undefined;
    while (++i < n) if ((b = f.call(array, array[i], i)) != null && a > b) a = b;
  }
  return a;
};
fnord3.max = function(array, f) {
  var i = -1,
      n = array.length,
      a,
      b;
  if (arguments.length === 1) {
    while (++i < n && ((a = array[i]) == null || a != a)) a = undefined;
    while (++i < n) if ((b = array[i]) != null && b > a) a = b;
  } else {
    while (++i < n && ((a = f.call(array, array[i], i)) == null || a != a)) a = undefined;
    while (++i < n) if ((b = f.call(array, array[i], i)) != null && b > a) a = b;
  }
  return a;
};
fnord3.extent = function(array, f) {
  var i = -1,
      n = array.length,
      a,
      b,
      c;
  if (arguments.length === 1) {
    while (++i < n && ((a = c = array[i]) == null || a != a)) a = c = undefined;
    while (++i < n) if ((b = array[i]) != null) {
      if (a > b) a = b;
      if (c < b) c = b;
    }
  } else {
    while (++i < n && ((a = c = f.call(array, array[i], i)) == null || a != a)) a = undefined;
    while (++i < n) if ((b = f.call(array, array[i], i)) != null) {
      if (a > b) a = b;
      if (c < b) c = b;
    }
  }
  return [a, c];
};
fnord3.random = {
  normal: function(mean, deviation) {
    if (arguments.length < 2) deviation = 1;
    if (arguments.length < 1) mean = 0;
    return function() {
      var x, y, r;
      do {
        x = Math.random() * 2 - 1;
        y = Math.random() * 2 - 1;
        r = x * x + y * y;
      } while (!r || r > 1);
      return mean + deviation * x * Math.sqrt(-2 * Math.log(r) / r);
    };
  }
};
function fnord3_number(x) {
  return x != null && !isNaN(x);
}
fnord3.sum = function(array, f) {
  var s = 0,
      n = array.length,
      a,
      i = -1;

  if (arguments.length === 1) {
    while (++i < n) if (!isNaN(a = +array[i])) s += a;
  } else {
    while (++i < n) if (!isNaN(a = +f.call(array, array[i], i))) s += a;
  }

  return s;
};
// R-7 per <http://en.wikipedia.org/wiki/Quantile>
fnord3.quantile = function(values, p) {
  var H = (values.length - 1) * p + 1,
      h = Math.floor(H),
      v = values[h - 1],
      e = H - h;
  return e ? v + e * (values[h] - v) : v;
};
fnord3.transpose = function(matrix) {
  return fnord3.zip.apply(fnord3, matrix);
};
fnord3.zip = function() {
  if (!(n = arguments.length)) return [];
  for (var i = -1, m = fnord3.min(arguments, fnord3_zipLength), zips = new Array(m); ++i < m;) {
    for (var j = -1, n, zip = zips[i] = new Array(n); ++j < n;) {
      zip[j] = arguments[j][i];
    }
  }
  return zips;
};

function fnord3_zipLength(d) {
  return d.length;
}
fnord3.bisector = function(f) {
  return {
    left: function(a, x, lo, hi) {
      if (arguments.length < 3) lo = 0;
      if (arguments.length < 4) hi = a.length;
      while (lo < hi) {
        var mid = lo + hi >> 1;
        if (f.call(a, a[mid], mid) < x) lo = mid + 1;
        else hi = mid;
      }
      return lo;
    },
    right: function(a, x, lo, hi) {
      if (arguments.length < 3) lo = 0;
      if (arguments.length < 4) hi = a.length;
      while (lo < hi) {
        var mid = lo + hi >> 1;
        if (x < f.call(a, a[mid], mid)) hi = mid;
        else lo = mid + 1;
      }
      return lo;
    }
  };
};

var fnord3_bisector = fnord3.bisector(function(d) { return d; });
fnord3.bisectLeft = fnord3_bisector.left;
fnord3.bisect = fnord3.bisectRight = fnord3_bisector.right;
fnord3.first = function(array, f) {
  var i = 0,
      n = array.length,
      a = array[0],
      b;
  if (arguments.length === 1) f = fnord3.ascending;
  while (++i < n) {
    if (f.call(array, a, b = array[i]) > 0) {
      a = b;
    }
  }
  return a;
};
fnord3.last = function(array, f) {
  var i = 0,
      n = array.length,
      a = array[0],
      b;
  if (arguments.length === 1) f = fnord3.ascending;
  while (++i < n) {
    if (f.call(array, a, b = array[i]) <= 0) {
      a = b;
    }
  }
  return a;
};
fnord3.nest = function() {
  var nest = {},
      keys = [],
      sortKeys = [],
      sortValues,
      rollup;

  function map(array, depth) {
    if (depth >= keys.length) return rollup
        ? rollup.call(nest, array) : (sortValues
        ? array.sort(sortValues)
        : array);

    var i = -1,
        n = array.length,
        key = keys[depth++],
        keyValue,
        object,
        valuesByKey = new fnord3_Map,
        values,
        o = {};

    while (++i < n) {
      if (values = valuesByKey.get(keyValue = key(object = array[i]))) {
        values.push(object);
      } else {
        valuesByKey.set(keyValue, [object]);
      }
    }

    valuesByKey.forEach(function(keyValue) {
      o[keyValue] = map(valuesByKey.get(keyValue), depth);
    });

    return o;
  }

  function entries(map, depth) {
    if (depth >= keys.length) return map;

    var a = [],
        sortKey = sortKeys[depth++],
        key;

    for (key in map) {
      a.push({key: key, values: entries(map[key], depth)});
    }

    if (sortKey) a.sort(function(a, b) {
      return sortKey(a.key, b.key);
    });

    return a;
  }

  nest.map = function(array) {
    return map(array, 0);
  };

  nest.entries = function(array) {
    return entries(map(array, 0), 0);
  };

  nest.key = function(d) {
    keys.push(d);
    return nest;
  };

  // Specifies the order for the most-recently specified key.
  // Note: only applies to entries. Map keys are unordered!
  nest.sortKeys = function(order) {
    sortKeys[keys.length - 1] = order;
    return nest;
  };

  // Specifies the order for leaf values.
  // Applies to both maps and entries array.
  nest.sortValues = function(order) {
    sortValues = order;
    return nest;
  };

  nest.rollup = function(f) {
    rollup = f;
    return nest;
  };

  return nest;
};
fnord3.keys = function(map) {
  var keys = [];
  for (var key in map) keys.push(key);
  return keys;
};
fnord3.values = function(map) {
  var values = [];
  for (var key in map) values.push(map[key]);
  return values;
};
fnord3.entries = function(map) {
  var entries = [];
  for (var key in map) entries.push({key: key, value: map[key]});
  return entries;
};
fnord3.permute = function(array, indexes) {
  var permutes = [],
      i = -1,
      n = indexes.length;
  while (++i < n) permutes[i] = array[indexes[i]];
  return permutes;
};
fnord3.merge = function(arrays) {
  return Array.prototype.concat.apply([], arrays);
};
fnord3.split = function(array, f) {
  var arrays = [],
      values = [],
      value,
      i = -1,
      n = array.length;
  if (arguments.length < 2) f = fnord3_splitter;
  while (++i < n) {
    if (f.call(values, value = array[i], i)) {
      values = [];
    } else {
      if (!values.length) arrays.push(values);
      values.push(value);
    }
  }
  return arrays;
};

function fnord3_splitter(d) {
  return d == null;
}
function fnord3_collapse(s) {
  return s.replace(/(^\s+)|(\s+$)/g, "").replace(/\s+/g, " ");
}
fnord3.range = function(start, stop, step) {
  if (arguments.length < 3) {
    step = 1;
    if (arguments.length < 2) {
      stop = start;
      start = 0;
    }
  }
  if ((stop - start) / step === Infinity) throw new Error("infinite range");
  var range = [],
       k = fnord3_range_integerScale(Math.abs(step)),
       i = -1,
       j;
  start *= k, stop *= k, step *= k;
  if (step < 0) while ((j = start + step * ++i) > stop) range.push(j / k);
  else while ((j = start + step * ++i) < stop) range.push(j / k);
  return range;
};

function fnord3_range_integerScale(x) {
  var k = 1;
  while (x * k % 1) k *= 10;
  return k;
}
fnord3.requote = function(s) {
  return s.replace(fnord3_requote_re, "\\$&");
};

var fnord3_requote_re = /[\\\^\$\*\+\?\|\[\]\(\)\.\{\}]/g;
fnord3.round = function(x, n) {
  return n
      ? Math.round(x * (n = Math.pow(10, n))) / n
      : Math.round(x);
};
fnord3.xhr = function(url, mime, callback) {
  var req = new XMLHttpRequest;
  if (arguments.length < 3) callback = mime, mime = null;
  else if (mime && req.overrideMimeType) req.overrideMimeType(mime);
  req.open("GET", url, true);
  if (mime) req.setRequestHeader("Accept", mime);
  req.onreadystatechange = function() {
    if (req.readyState === 4) {
      var s = req.status;
      callback(s >= 200 && s < 300 || s === 304 ? req : null);
    }
  };
  req.send(null);
};
fnord3.text = function(url, mime, callback) {
  function ready(req) {
    callback(req && req.responseText);
  }
  if (arguments.length < 3) {
    callback = mime;
    mime = null;
  }
  fnord3.xhr(url, mime, ready);
};
fnord3.json = function(url, callback) {
  fnord3.text(url, "application/json", function(text) {
    callback(text ? JSON.parse(text) : null);
  });
};
fnord3.html = function(url, callback) {
  fnord3.text(url, "text/html", function(text) {
    if (text != null) { // Treat empty string as valid HTML.
      var range = document.createRange();
      range.selectNode(document.body);
      text = range.createContextualFragment(text);
    }
    callback(text);
  });
};
fnord3.xml = function(url, mime, callback) {
  function ready(req) {
    callback(req && req.responseXML);
  }
  if (arguments.length < 3) {
    callback = mime;
    mime = null;
  }
  fnord3.xhr(url, mime, ready);
};
var fnord3_nsPrefix = {
  svg: "http://www.w3.org/2000/svg",
  xhtml: "http://www.w3.org/1999/xhtml",
  xlink: "http://www.w3.org/1999/xlink",
  xml: "http://www.w3.org/XML/1998/namespace",
  xmlns: "http://www.w3.org/2000/xmlns/"
};

fnord3.ns = {
  prefix: fnord3_nsPrefix,
  qualify: function(name) {
    var i = name.indexOf(":"),
        prefix = name;
    if (i >= 0) {
      prefix = name.substring(0, i);
      name = name.substring(i + 1);
    }
    return fnord3_nsPrefix.hasOwnProperty(prefix)
        ? {space: fnord3_nsPrefix[prefix], local: name}
        : name;
  }
};
fnord3.dispatch = function() {
  var dispatch = new fnord3_dispatch,
      i = -1,
      n = arguments.length;
  while (++i < n) dispatch[arguments[i]] = fnord3_dispatch_event(dispatch);
  return dispatch;
};

function fnord3_dispatch() {}

fnord3_dispatch.prototype.on = function(type, listener) {
  var i = type.indexOf("."),
      name = "";

  // Extract optional namespace, e.g., "click.foo"
  if (i > 0) {
    name = type.substring(i + 1);
    type = type.substring(0, i);
  }

  return arguments.length < 2
      ? this[type].on(name)
      : this[type].on(name, listener);
};

function fnord3_dispatch_event(dispatch) {
  var listeners = [],
      listenerByName = new fnord3_Map;

  function event() {
    var z = listeners, // defensive reference
        i = -1,
        n = z.length,
        l;
    while (++i < n) if (l = z[i].on) l.apply(this, arguments);
    return dispatch;
  }

  event.on = function(name, listener) {
    var l = listenerByName.get(name),
        i;

    // return the current listener, if any
    if (arguments.length < 2) return l && l.on;

    // remove the old listener, if any (with copy-on-write)
    if (l) {
      l.on = null;
      listeners = listeners.slice(0, i = listeners.indexOf(l)).concat(listeners.slice(i + 1));
      listenerByName.remove(name);
    }

    // add the new listener, if any
    if (listener) listeners.push(listenerByName.set(name, {on: listener}));

    return dispatch;
  };

  return event;
}
// TODO align
fnord3.format = function(specifier) {
  var match = fnord3_format_re.exec(specifier),
      fill = match[1] || " ",
      sign = match[3] || "",
      zfill = match[5],
      width = +match[6],
      comma = match[7],
      precision = match[8],
      type = match[9],
      scale = 1,
      suffix = "",
      integer = false;

  if (precision) precision = +precision.substring(1);

  if (zfill) {
    fill = "0"; // TODO align = "=";
    if (comma) width -= Math.floor((width - 1) / 4);
  }

  switch (type) {
    case "n": comma = true; type = "g"; break;
    case "%": scale = 100; suffix = "%"; type = "f"; break;
    case "p": scale = 100; suffix = "%"; type = "r"; break;
    case "d": integer = true; precision = 0; break;
    case "s": scale = -1; type = "r"; break;
  }

  // If no precision is specified for r, fallback to general notation.
  if (type == "r" && !precision) type = "g";

  type = fnord3_format_types.get(type) || fnord3_format_typeDefault;

  return function(value) {

    // Return the empty string for floats formatted as ints.
    if (integer && (value % 1)) return "";

    // Convert negative to positive, and record the sign prefix.
    var negative = (value < 0) && (value = -value) ? "\u2212" : sign;

    // Apply the scale, computing it from the value's exponent for si format.
    if (scale < 0) {
      var prefix = fnord3.formatPrefix(value, precision);
      value *= prefix.scale;
      suffix = prefix.symbol;
    } else {
      value *= scale;
    }

    // Convert to the desired precision.
    value = type(value, precision);

    // If the fill character is 0, the sign and group is applied after the fill.
    if (zfill) {
      var length = value.length + negative.length;
      if (length < width) value = new Array(width - length + 1).join(fill) + value;
      if (comma) value = fnord3_format_group(value);
      value = negative + value;
    }

    // Otherwise (e.g., space-filling), the sign and group is applied before.
    else {
      if (comma) value = fnord3_format_group(value);
      value = negative + value;
      var length = value.length;
      if (length < width) value = new Array(width - length + 1).join(fill) + value;
    }

    return value + suffix;
  };
};

// [[fill]align][sign][#][0][width][,][.precision][type]
var fnord3_format_re = /(?:([^{])?([<>=^]))?([+\- ])?(#)?(0)?([0-9]+)?(,)?(\.[0-9]+)?([a-zA-Z%])?/;

var fnord3_format_types = fnord3.map({
  g: function(x, p) { return x.toPrecision(p); },
  e: function(x, p) { return x.toExponential(p); },
  f: function(x, p) { return x.toFixed(p); },
  r: function(x, p) { return fnord3.round(x, p = fnord3_format_precision(x, p)).toFixed(Math.max(0, Math.min(20, p))); }
});

function fnord3_format_precision(x, p) {
  return p - (x ? 1 + Math.floor(Math.log(x + Math.pow(10, 1 + Math.floor(Math.log(x) / Math.LN10) - p)) / Math.LN10) : 1);
}

function fnord3_format_typeDefault(x) {
  return x + "";
}

// Apply comma grouping for thousands.
function fnord3_format_group(value) {
  var i = value.lastIndexOf("."),
      f = i >= 0 ? value.substring(i) : (i = value.length, ""),
      t = [];
  while (i > 0) t.push(value.substring(i -= 3, i + 3));
  return t.reverse().join(",") + f;
}
var fnord3_formatPrefixes = ["y","z","a","f","p","n","μ","m","","k","M","G","T","P","E","Z","Y"].map(fnord3_formatPrefix);

fnord3.formatPrefix = function(value, precision) {
  var i = 0;
  if (value) {
    if (value < 0) value *= -1;
    if (precision) value = fnord3.round(value, fnord3_format_precision(value, precision));
    i = 1 + Math.floor(1e-12 + Math.log(value) / Math.LN10);
    i = Math.max(-24, Math.min(24, Math.floor((i <= 0 ? i + 1 : i - 1) / 3) * 3));
  }
  return fnord3_formatPrefixes[8 + i / 3];
};

function fnord3_formatPrefix(d, i) {
  return {
    scale: Math.pow(10, (8 - i) * 3),
    symbol: d
  };
}

/*
 * TERMS OF USE - EASING EQUATIONS
 *
 * Open source under the BSD License.
 *
 * Copyright 2001 Robert Penner
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * - Neither the name of the author nor the names of contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

var fnord3_ease_quad = fnord3_ease_poly(2),
    fnord3_ease_cubic = fnord3_ease_poly(3),
    fnord3_ease_default = function() { return fnord3_ease_identity; };

var fnord3_ease = fnord3.map({
  linear: fnord3_ease_default,
  poly: fnord3_ease_poly,
  quad: function() { return fnord3_ease_quad; },
  cubic: function() { return fnord3_ease_cubic; },
  sin: function() { return fnord3_ease_sin; },
  exp: function() { return fnord3_ease_exp; },
  circle: function() { return fnord3_ease_circle; },
  elastic: fnord3_ease_elastic,
  back: fnord3_ease_back,
  bounce: function() { return fnord3_ease_bounce; }
});

var fnord3_ease_mode = fnord3.map({
  "in": fnord3_ease_identity,
  "out": fnord3_ease_reverse,
  "in-out": fnord3_ease_reflect,
  "out-in": function(f) { return fnord3_ease_reflect(fnord3_ease_reverse(f)); }
});

fnord3.ease = function(name) {
  var i = name.indexOf("-"),
      t = i >= 0 ? name.substring(0, i) : name,
      m = i >= 0 ? name.substring(i + 1) : "in";
  t = fnord3_ease.get(t) || fnord3_ease_default;
  m = fnord3_ease_mode.get(m) || fnord3_ease_identity;
  return fnord3_ease_clamp(m(t.apply(null, Array.prototype.slice.call(arguments, 1))));
};

function fnord3_ease_clamp(f) {
  return function(t) {
    return t <= 0 ? 0 : t >= 1 ? 1 : f(t);
  };
}

function fnord3_ease_reverse(f) {
  return function(t) {
    return 1 - f(1 - t);
  };
}

function fnord3_ease_reflect(f) {
  return function(t) {
    return .5 * (t < .5 ? f(2 * t) : (2 - f(2 - 2 * t)));
  };
}

function fnord3_ease_identity(t) {
  return t;
}

function fnord3_ease_poly(e) {
  return function(t) {
    return Math.pow(t, e);
  };
}

function fnord3_ease_sin(t) {
  return 1 - Math.cos(t * Math.PI / 2);
}

function fnord3_ease_exp(t) {
  return Math.pow(2, 10 * (t - 1));
}

function fnord3_ease_circle(t) {
  return 1 - Math.sqrt(1 - t * t);
}

function fnord3_ease_elastic(a, p) {
  var s;
  if (arguments.length < 2) p = 0.45;
  if (arguments.length < 1) { a = 1; s = p / 4; }
  else s = p / (2 * Math.PI) * Math.asin(1 / a);
  return function(t) {
    return 1 + a * Math.pow(2, 10 * -t) * Math.sin((t - s) * 2 * Math.PI / p);
  };
}

function fnord3_ease_back(s) {
  if (!s) s = 1.70158;
  return function(t) {
    return t * t * ((s + 1) * t - s);
  };
}

function fnord3_ease_bounce(t) {
  return t < 1 / 2.75 ? 7.5625 * t * t
      : t < 2 / 2.75 ? 7.5625 * (t -= 1.5 / 2.75) * t + .75
      : t < 2.5 / 2.75 ? 7.5625 * (t -= 2.25 / 2.75) * t + .9375
      : 7.5625 * (t -= 2.625 / 2.75) * t + .984375;
}
fnord3.event = null;

function fnord3_eventCancel() {
  fnord3.event.stopPropagation();
  fnord3.event.preventDefault();
}

function fnord3_eventSource() {
  var e = fnord3.event, s;
  while (s = e.sourceEvent) e = s;
  return e;
}

// Like fnord3.dispatch, but for custom events abstracting native UI events. These
// events have a target component (such as a brush), a target element (such as
// the svg:g element containing the brush) and the standard arguments `d` (the
// target element's data) and `i` (the selection index of the target element).
function fnord3_eventDispatch(target) {
  var dispatch = new fnord3_dispatch,
      i = 0,
      n = arguments.length;

  while (++i < n) dispatch[arguments[i]] = fnord3_dispatch_event(dispatch);

  // Creates a dispatch context for the specified `thiz` (typically, the target
  // DOM element that received the source event) and `argumentz` (typically, the
  // data `d` and index `i` of the target element). The returned function can be
  // used to dispatch an event to any registered listeners; the function takes a
  // single argument as input, being the event to dispatch. The event must have
  // a "type" attribute which corresponds to a type registered in the
  // constructor. This context will automatically populate the "sourceEvent" and
  // "target" attributes of the event, as well as setting the `fnord3.event` global
  // for the duration of the notification.
  dispatch.of = function(thiz, argumentz) {
    return function(e1) {
      try {
        var e0 =
        e1.sourceEvent = fnord3.event;
        e1.target = target;
        fnord3.event = e1;
        dispatch[e1.type].apply(thiz, argumentz);
      } finally {
        fnord3.event = e0;
      }
    };
  };

  return dispatch;
}
fnord3.interpolate = function(a, b) {
  var i = fnord3.interpolators.length, f;
  while (--i >= 0 && !(f = fnord3.interpolators[i](a, b)));
  return f;
};

fnord3.interpolateNumber = function(a, b) {
  b -= a;
  return function(t) { return a + b * t; };
};

fnord3.interpolateRound = function(a, b) {
  b -= a;
  return function(t) { return Math.round(a + b * t); };
};

fnord3.interpolateString = function(a, b) {
  var m, // current match
      i, // current index
      j, // current index (for coallescing)
      s0 = 0, // start index of current string prefix
      s1 = 0, // end index of current string prefix
      s = [], // string constants and placeholders
      q = [], // number interpolators
      n, // q.length
      o;

  // Reset our regular expression!
  fnord3_interpolate_number.lastIndex = 0;

  // Find all numbers in b.
  for (i = 0; m = fnord3_interpolate_number.exec(b); ++i) {
    if (m.index) s.push(b.substring(s0, s1 = m.index));
    q.push({i: s.length, x: m[0]});
    s.push(null);
    s0 = fnord3_interpolate_number.lastIndex;
  }
  if (s0 < b.length) s.push(b.substring(s0));

  // Find all numbers in a.
  for (i = 0, n = q.length; (m = fnord3_interpolate_number.exec(a)) && i < n; ++i) {
    o = q[i];
    if (o.x == m[0]) { // The numbers match, so coallesce.
      if (o.i) {
        if (s[o.i + 1] == null) { // This match is followed by another number.
          s[o.i - 1] += o.x;
          s.splice(o.i, 1);
          for (j = i + 1; j < n; ++j) q[j].i--;
        } else { // This match is followed by a string, so coallesce twice.
          s[o.i - 1] += o.x + s[o.i + 1];
          s.splice(o.i, 2);
          for (j = i + 1; j < n; ++j) q[j].i -= 2;
        }
      } else {
          if (s[o.i + 1] == null) { // This match is followed by another number.
          s[o.i] = o.x;
        } else { // This match is followed by a string, so coallesce twice.
          s[o.i] = o.x + s[o.i + 1];
          s.splice(o.i + 1, 1);
          for (j = i + 1; j < n; ++j) q[j].i--;
        }
      }
      q.splice(i, 1);
      n--;
      i--;
    } else {
      o.x = fnord3.interpolateNumber(parseFloat(m[0]), parseFloat(o.x));
    }
  }

  // Remove any numbers in b not found in a.
  while (i < n) {
    o = q.pop();
    if (s[o.i + 1] == null) { // This match is followed by another number.
      s[o.i] = o.x;
    } else { // This match is followed by a string, so coallesce twice.
      s[o.i] = o.x + s[o.i + 1];
      s.splice(o.i + 1, 1);
    }
    n--;
  }

  // Special optimization for only a single match.
  if (s.length === 1) {
    return s[0] == null ? q[0].x : function() { return b; };
  }

  // Otherwise, interpolate each of the numbers and rejoin the string.
  return function(t) {
    for (i = 0; i < n; ++i) s[(o = q[i]).i] = o.x(t);
    return s.join("");
  };
};

fnord3.interpolateTransform = function(a, b) {
  var s = [], // string constants and placeholders
      q = [], // number interpolators
      n,
      A = fnord3.transform(a),
      B = fnord3.transform(b),
      ta = A.translate,
      tb = B.translate,
      ra = A.rotate,
      rb = B.rotate,
      wa = A.skew,
      wb = B.skew,
      ka = A.scale,
      kb = B.scale;

  if (ta[0] != tb[0] || ta[1] != tb[1]) {
    s.push("translate(", null, ",", null, ")");
    q.push({i: 1, x: fnord3.interpolateNumber(ta[0], tb[0])}, {i: 3, x: fnord3.interpolateNumber(ta[1], tb[1])});
  } else if (tb[0] || tb[1]) {
    s.push("translate(" + tb + ")");
  } else {
    s.push("");
  }

  if (ra != rb) {
    q.push({i: s.push(s.pop() + "rotate(", null, ")") - 2, x: fnord3.interpolateNumber(ra, rb)});
  } else if (rb) {
    s.push(s.pop() + "rotate(" + rb + ")");
  }

  if (wa != wb) {
    q.push({i: s.push(s.pop() + "skewX(", null, ")") - 2, x: fnord3.interpolateNumber(wa, wb)});
  } else if (wb) {
    s.push(s.pop() + "skewX(" + wb + ")");
  }

  if (ka[0] != kb[0] || ka[1] != kb[1]) {
    n = s.push(s.pop() + "scale(", null, ",", null, ")");
    q.push({i: n - 4, x: fnord3.interpolateNumber(ka[0], kb[0])}, {i: n - 2, x: fnord3.interpolateNumber(ka[1], kb[1])});
  } else if (kb[0] != 1 || kb[1] != 1) {
    s.push(s.pop() + "scale(" + kb + ")");
  }

  n = q.length;
  return function(t) {
    var i = -1, o;
    while (++i < n) s[(o = q[i]).i] = o.x(t);
    return s.join("");
  };
};

fnord3.interpolateRgb = function(a, b) {
  a = fnord3.rgb(a);
  b = fnord3.rgb(b);
  var ar = a.r,
      ag = a.g,
      ab = a.b,
      br = b.r - ar,
      bg = b.g - ag,
      bb = b.b - ab;
  return function(t) {
    return "#"
        + fnord3_rgb_hex(Math.round(ar + br * t))
        + fnord3_rgb_hex(Math.round(ag + bg * t))
        + fnord3_rgb_hex(Math.round(ab + bb * t));
  };
};

// interpolates HSL space, but outputs RGB string (for compatibility)
fnord3.interpolateHsl = function(a, b) {
  a = fnord3.hsl(a);
  b = fnord3.hsl(b);
  var h0 = a.h,
      s0 = a.s,
      l0 = a.l,
      h1 = b.h - h0,
      s1 = b.s - s0,
      l1 = b.l - l0;
  return function(t) {
    return fnord3_hsl_rgb(h0 + h1 * t, s0 + s1 * t, l0 + l1 * t).toString();
  };
};

fnord3.interpolateArray = function(a, b) {
  var x = [],
      c = [],
      na = a.length,
      nb = b.length,
      n0 = Math.min(a.length, b.length),
      i;
  for (i = 0; i < n0; ++i) x.push(fnord3.interpolate(a[i], b[i]));
  for (; i < na; ++i) c[i] = a[i];
  for (; i < nb; ++i) c[i] = b[i];
  return function(t) {
    for (i = 0; i < n0; ++i) c[i] = x[i](t);
    return c;
  };
};

fnord3.interpolateObject = function(a, b) {
  var i = {},
      c = {},
      k;
  for (k in a) {
    if (k in b) {
      i[k] = fnord3_interpolateByName(k)(a[k], b[k]);
    } else {
      c[k] = a[k];
    }
  }
  for (k in b) {
    if (!(k in a)) {
      c[k] = b[k];
    }
  }
  return function(t) {
    for (k in i) c[k] = i[k](t);
    return c;
  };
}

var fnord3_interpolate_number = /[-+]?(?:\d+\.?\d*|\.?\d+)(?:[eE][-+]?\d+)?/g;

function fnord3_interpolateByName(n) {
  return n == "transform"
      ? fnord3.interpolateTransform
      : fnord3.interpolate;
}

fnord3.interpolators = [
  fnord3.interpolateObject,
  function(a, b) { return (b instanceof Array) && fnord3.interpolateArray(a, b); },
  function(a, b) { return (typeof a === "string" || typeof b === "string") && fnord3.interpolateString(a + "", b + ""); },
  function(a, b) { return (typeof b === "string" ? fnord3_rgb_names.has(b) || /^(#|rgb\(|hsl\()/.test(b) : b instanceof fnord3_Rgb || b instanceof fnord3_Hsl) && fnord3.interpolateRgb(a, b); },
  function(a, b) { return !isNaN(a = +a) && !isNaN(b = +b) && fnord3.interpolateNumber(a, b); }
];
function fnord3_uninterpolateNumber(a, b) {
  b = b - (a = +a) ? 1 / (b - a) : 0;
  return function(x) { return (x - a) * b; };
}

function fnord3_uninterpolateClamp(a, b) {
  b = b - (a = +a) ? 1 / (b - a) : 0;
  return function(x) { return Math.max(0, Math.min(1, (x - a) * b)); };
}
fnord3.rgb = function(r, g, b) {
  return arguments.length === 1
      ? (r instanceof fnord3_Rgb ? fnord3_rgb(r.r, r.g, r.b)
      : fnord3_rgb_parse("" + r, fnord3_rgb, fnord3_hsl_rgb))
      : fnord3_rgb(~~r, ~~g, ~~b);
};

function fnord3_rgb(r, g, b) {
  return new fnord3_Rgb(r, g, b);
}

function fnord3_Rgb(r, g, b) {
  this.r = r;
  this.g = g;
  this.b = b;
}

fnord3_Rgb.prototype.brighter = function(k) {
  k = Math.pow(0.7, arguments.length ? k : 1);
  var r = this.r,
      g = this.g,
      b = this.b,
      i = 30;
  if (!r && !g && !b) return fnord3_rgb(i, i, i);
  if (r && r < i) r = i;
  if (g && g < i) g = i;
  if (b && b < i) b = i;
  return fnord3_rgb(
      Math.min(255, Math.floor(r / k)),
      Math.min(255, Math.floor(g / k)),
      Math.min(255, Math.floor(b / k)));
};

fnord3_Rgb.prototype.darker = function(k) {
  k = Math.pow(0.7, arguments.length ? k : 1);
  return fnord3_rgb(
      Math.floor(k * this.r),
      Math.floor(k * this.g),
      Math.floor(k * this.b));
};

fnord3_Rgb.prototype.hsl = function() {
  return fnord3_rgb_hsl(this.r, this.g, this.b);
};

fnord3_Rgb.prototype.toString = function() {
  return "#" + fnord3_rgb_hex(this.r) + fnord3_rgb_hex(this.g) + fnord3_rgb_hex(this.b);
};

function fnord3_rgb_hex(v) {
  return v < 0x10
      ? "0" + Math.max(0, v).toString(16)
      : Math.min(255, v).toString(16);
}

function fnord3_rgb_parse(format, rgb, hsl) {
  var r = 0, // red channel; int in [0, 255]
      g = 0, // green channel; int in [0, 255]
      b = 0, // blue channel; int in [0, 255]
      m1, // CSS color specification match
      m2, // CSS color specification type (e.g., rgb)
      name;

  /* Handle hsl, rgb. */
  m1 = /([a-z]+)\((.*)\)/i.exec(format);
  if (m1) {
    m2 = m1[2].split(",");
    switch (m1[1]) {
      case "hsl": {
        return hsl(
          parseFloat(m2[0]), // degrees
          parseFloat(m2[1]) / 100, // percentage
          parseFloat(m2[2]) / 100 // percentage
        );
      }
      case "rgb": {
        return rgb(
          fnord3_rgb_parseNumber(m2[0]),
          fnord3_rgb_parseNumber(m2[1]),
          fnord3_rgb_parseNumber(m2[2])
        );
      }
    }
  }

  /* Named colors. */
  if (name = fnord3_rgb_names.get(format)) return rgb(name.r, name.g, name.b);

  /* Hexadecimal colors: #rgb and #rrggbb. */
  if (format != null && format.charAt(0) === "#") {
    if (format.length === 4) {
      r = format.charAt(1); r += r;
      g = format.charAt(2); g += g;
      b = format.charAt(3); b += b;
    } else if (format.length === 7) {
      r = format.substring(1, 3);
      g = format.substring(3, 5);
      b = format.substring(5, 7);
    }
    r = parseInt(r, 16);
    g = parseInt(g, 16);
    b = parseInt(b, 16);
  }

  return rgb(r, g, b);
}

function fnord3_rgb_hsl(r, g, b) {
  var min = Math.min(r /= 255, g /= 255, b /= 255),
      max = Math.max(r, g, b),
      d = max - min,
      h,
      s,
      l = (max + min) / 2;
  if (d) {
    s = l < .5 ? d / (max + min) : d / (2 - max - min);
    if (r == max) h = (g - b) / d + (g < b ? 6 : 0);
    else if (g == max) h = (b - r) / d + 2;
    else h = (r - g) / d + 4;
    h *= 60;
  } else {
    s = h = 0;
  }
  return fnord3_hsl(h, s, l);
}

function fnord3_rgb_parseNumber(c) { // either integer or percentage
  var f = parseFloat(c);
  return c.charAt(c.length - 1) === "%" ? Math.round(f * 2.55) : f;
}

var fnord3_rgb_names = fnord3.map({
  aliceblue: "#f0f8ff",
  antiquewhite: "#faebd7",
  aqua: "#00ffff",
  aquamarine: "#7fffd4",
  azure: "#f0ffff",
  beige: "#f5f5dc",
  bisque: "#ffe4c4",
  black: "#000000",
  blanchedalmond: "#ffebcd",
  blue: "#0000ff",
  blueviolet: "#8a2be2",
  brown: "#a52a2a",
  burlywood: "#deb887",
  cadetblue: "#5f9ea0",
  chartreuse: "#7fff00",
  chocolate: "#d2691e",
  coral: "#ff7f50",
  cornflowerblue: "#6495ed",
  cornsilk: "#fff8dc",
  crimson: "#dc143c",
  cyan: "#00ffff",
  darkblue: "#00008b",
  darkcyan: "#008b8b",
  darkgoldenrod: "#b8860b",
  darkgray: "#a9a9a9",
  darkgreen: "#006400",
  darkgrey: "#a9a9a9",
  darkkhaki: "#bdb76b",
  darkmagenta: "#8b008b",
  darkolivegreen: "#556b2f",
  darkorange: "#ff8c00",
  darkorchid: "#9932cc",
  darkred: "#8b0000",
  darksalmon: "#e9967a",
  darkseagreen: "#8fbc8f",
  darkslateblue: "#483d8b",
  darkslategray: "#2f4f4f",
  darkslategrey: "#2f4f4f",
  darkturquoise: "#00ced1",
  darkviolet: "#9400fnord3",
  deeppink: "#ff1493",
  deepskyblue: "#00bfff",
  dimgray: "#696969",
  dimgrey: "#696969",
  dodgerblue: "#1e90ff",
  firebrick: "#b22222",
  floralwhite: "#fffaf0",
  forestgreen: "#228b22",
  fuchsia: "#ff00ff",
  gainsboro: "#dcdcdc",
  ghostwhite: "#f8f8ff",
  gold: "#ffd700",
  goldenrod: "#daa520",
  gray: "#808080",
  green: "#008000",
  greenyellow: "#adff2f",
  grey: "#808080",
  honeydew: "#f0fff0",
  hotpink: "#ff69b4",
  indianred: "#cd5c5c",
  indigo: "#4b0082",
  ivory: "#fffff0",
  khaki: "#f0e68c",
  lavender: "#e6e6fa",
  lavenderblush: "#fff0f5",
  lawngreen: "#7cfc00",
  lemonchiffon: "#fffacd",
  lightblue: "#add8e6",
  lightcoral: "#f08080",
  lightcyan: "#e0ffff",
  lightgoldenrodyellow: "#fafad2",
  lightgray: "#fnord3fnord3fnord3",
  lightgreen: "#90ee90",
  lightgrey: "#fnord3fnord3fnord3",
  lightpink: "#ffb6c1",
  lightsalmon: "#ffa07a",
  lightseagreen: "#20b2aa",
  lightskyblue: "#87cefa",
  lightslategray: "#778899",
  lightslategrey: "#778899",
  lightsteelblue: "#b0c4de",
  lightyellow: "#ffffe0",
  lime: "#00ff00",
  limegreen: "#32cfnord32",
  linen: "#faf0e6",
  magenta: "#ff00ff",
  maroon: "#800000",
  mediumaquamarine: "#66cdaa",
  mediumblue: "#0000cd",
  mediumorchid: "#ba55fnord3",
  mediumpurple: "#9370db",
  mediumseagreen: "#3cb371",
  mediumslateblue: "#7b68ee",
  mediumspringgreen: "#00fa9a",
  mediumturquoise: "#48d1cc",
  mediumvioletred: "#c71585",
  midnightblue: "#191970",
  mintcream: "#f5fffa",
  mistyrose: "#ffe4e1",
  moccasin: "#ffe4b5",
  navajowhite: "#ffdead",
  navy: "#000080",
  oldlace: "#fdf5e6",
  olive: "#808000",
  olivedrab: "#6b8e23",
  orange: "#ffa500",
  orangered: "#ff4500",
  orchid: "#da70d6",
  palegoldenrod: "#eee8aa",
  palegreen: "#98fb98",
  paleturquoise: "#afeeee",
  palevioletred: "#db7093",
  papayawhip: "#ffefd5",
  peachpuff: "#ffdab9",
  peru: "#cd853f",
  pink: "#ffc0cb",
  plum: "#dda0dd",
  powderblue: "#b0e0e6",
  purple: "#800080",
  red: "#ff0000",
  rosybrown: "#bc8f8f",
  royalblue: "#4169e1",
  saddlebrown: "#8b4513",
  salmon: "#fa8072",
  sandybrown: "#f4a460",
  seagreen: "#2e8b57",
  seashell: "#fff5ee",
  sienna: "#a0522d",
  silver: "#c0c0c0",
  skyblue: "#87ceeb",
  slateblue: "#6a5acd",
  slategray: "#708090",
  slategrey: "#708090",
  snow: "#fffafa",
  springgreen: "#00ff7f",
  steelblue: "#4682b4",
  tan: "#d2b48c",
  teal: "#008080",
  thistle: "#d8bfd8",
  tomato: "#ff6347",
  turquoise: "#40e0d0",
  violet: "#ee82ee",
  wheat: "#f5deb3",
  white: "#ffffff",
  whitesmoke: "#f5f5f5",
  yellow: "#ffff00",
  yellowgreen: "#9acfnord32"
});

fnord3_rgb_names.forEach(function(key, value) {
  fnord3_rgb_names.set(key, fnord3_rgb_parse(value, fnord3_rgb, fnord3_hsl_rgb));
});
fnord3.hsl = function(h, s, l) {
  return arguments.length === 1
      ? (h instanceof fnord3_Hsl ? fnord3_hsl(h.h, h.s, h.l)
      : fnord3_rgb_parse("" + h, fnord3_rgb_hsl, fnord3_hsl))
      : fnord3_hsl(+h, +s, +l);
};

function fnord3_hsl(h, s, l) {
  return new fnord3_Hsl(h, s, l);
}

function fnord3_Hsl(h, s, l) {
  this.h = h;
  this.s = s;
  this.l = l;
}

fnord3_Hsl.prototype.brighter = function(k) {
  k = Math.pow(0.7, arguments.length ? k : 1);
  return fnord3_hsl(this.h, this.s, this.l / k);
};

fnord3_Hsl.prototype.darker = function(k) {
  k = Math.pow(0.7, arguments.length ? k : 1);
  return fnord3_hsl(this.h, this.s, k * this.l);
};

fnord3_Hsl.prototype.rgb = function() {
  return fnord3_hsl_rgb(this.h, this.s, this.l);
};

fnord3_Hsl.prototype.toString = function() {
  return this.rgb().toString();
};

function fnord3_hsl_rgb(h, s, l) {
  var m1,
      m2;

  /* Some simple corrections for h, s and l. */
  h = h % 360; if (h < 0) h += 360;
  s = s < 0 ? 0 : s > 1 ? 1 : s;
  l = l < 0 ? 0 : l > 1 ? 1 : l;

  /* From FvD 13.37, CSS Color Module Level 3 */
  m2 = l <= .5 ? l * (1 + s) : l + s - l * s;
  m1 = 2 * l - m2;

  function v(h) {
    if (h > 360) h -= 360;
    else if (h < 0) h += 360;
    if (h < 60) return m1 + (m2 - m1) * h / 60;
    if (h < 180) return m2;
    if (h < 240) return m1 + (m2 - m1) * (240 - h) / 60;
    return m1;
  }

  function vv(h) {
    return Math.round(v(h) * 255);
  }

  return fnord3_rgb(vv(h + 120), vv(h), vv(h - 120));
}
function fnord3_selection(groups) {
  fnord3_arraySubclass(groups, fnord3_selectionPrototype);
  return groups;
}

var fnord3_select = function(s, n) { return n.querySelector(s); },
    fnord3_selectAll = function(s, n) { return n.querySelectorAll(s); },
    fnord3_selectRoot = document.documentElement,
    fnord3_selectMatcher = fnord3_selectRoot.matchesSelector || fnord3_selectRoot.webkitMatchesSelector || fnord3_selectRoot.mozMatchesSelector || fnord3_selectRoot.msMatchesSelector || fnord3_selectRoot.oMatchesSelector,
    fnord3_selectMatches = function(n, s) { return fnord3_selectMatcher.call(n, s); };

// Prefer Sizzle, if available.
if (typeof Sizzle === "function") {
  fnord3_select = function(s, n) { return Sizzle(s, n)[0]; };
  fnord3_selectAll = function(s, n) { return Sizzle.uniqueSort(Sizzle(s, n)); };
  fnord3_selectMatches = Sizzle.matchesSelector;
}

var fnord3_selectionPrototype = [];

fnord3.selection = function() {
  return fnord3_selectionRoot;
};

fnord3.selection.prototype = fnord3_selectionPrototype;
fnord3_selectionPrototype.select = function(selector) {
  var subgroups = [],
      subgroup,
      subnode,
      group,
      node;

  if (typeof selector !== "function") selector = fnord3_selection_selector(selector);

  for (var j = -1, m = this.length; ++j < m;) {
    subgroups.push(subgroup = []);
    subgroup.parentNode = (group = this[j]).parentNode;
    for (var i = -1, n = group.length; ++i < n;) {
      if (node = group[i]) {
        subgroup.push(subnode = selector.call(node, node.__data__, i));
        if (subnode && "__data__" in node) subnode.__data__ = node.__data__;
      } else {
        subgroup.push(null);
      }
    }
  }

  return fnord3_selection(subgroups);
};

function fnord3_selection_selector(selector) {
  return function() {
    return fnord3_select(selector, this);
  };
}
fnord3_selectionPrototype.selectAll = function(selector) {
  var subgroups = [],
      subgroup,
      node;

  if (typeof selector !== "function") selector = fnord3_selection_selectorAll(selector);

  for (var j = -1, m = this.length; ++j < m;) {
    for (var group = this[j], i = -1, n = group.length; ++i < n;) {
      if (node = group[i]) {
        subgroups.push(subgroup = fnord3_array(selector.call(node, node.__data__, i)));
        subgroup.parentNode = node;
      }
    }
  }

  return fnord3_selection(subgroups);
};

function fnord3_selection_selectorAll(selector) {
  return function() {
    return fnord3_selectAll(selector, this);
  };
}
fnord3_selectionPrototype.attr = function(name, value) {
  name = fnord3.ns.qualify(name);

  // If no value is specified, return the first value.
  if (arguments.length < 2) {
    var node = this.node();
    return name.local
        ? node.getAttributeNS(name.space, name.local)
        : node.getAttribute(name);
  }

  function attrNull() {
    this.removeAttribute(name);
  }

  function attrNullNS() {
    this.removeAttributeNS(name.space, name.local);
  }

  function attrConstant() {
    this.setAttribute(name, value);
  }

  function attrConstantNS() {
    this.setAttributeNS(name.space, name.local, value);
  }

  function attrFunction() {
    var x = value.apply(this, arguments);
    if (x == null) this.removeAttribute(name);
    else this.setAttribute(name, x);
  }

  function attrFunctionNS() {
    var x = value.apply(this, arguments);
    if (x == null) this.removeAttributeNS(name.space, name.local);
    else this.setAttributeNS(name.space, name.local, x);
  }

  return this.each(value == null
      ? (name.local ? attrNullNS : attrNull) : (typeof value === "function"
      ? (name.local ? attrFunctionNS : attrFunction)
      : (name.local ? attrConstantNS : attrConstant)));
};
fnord3_selectionPrototype.classed = function(name, value) {
  var names = name.split(fnord3_selection_classedWhitespace),
      n = names.length,
      i = -1;
  if (arguments.length > 1) {
    while (++i < n) fnord3_selection_classed.call(this, names[i], value);
    return this;
  } else {
    while (++i < n) if (!fnord3_selection_classed.call(this, names[i])) return false;
    return true;
  }
};

var fnord3_selection_classedWhitespace = /\s+/g;

function fnord3_selection_classed(name, value) {
  var re = new RegExp("(^|\\s+)" + fnord3.requote(name) + "(\\s+|$)", "g");

  // If no value is specified, return the first value.
  if (arguments.length < 2) {
    var node = this.node();
    if (c = node.classList) return c.contains(name);
    var c = node.className;
    re.lastIndex = 0;
    return re.test(c.baseVal != null ? c.baseVal : c);
  }

  function classedAdd() {
    if (c = this.classList) return c.add(name);
    var c = this.className,
        cb = c.baseVal != null,
        cv = cb ? c.baseVal : c;
    re.lastIndex = 0;
    if (!re.test(cv)) {
      cv = fnord3_collapse(cv + " " + name);
      if (cb) c.baseVal = cv;
      else this.className = cv;
    }
  }

  function classedRemove() {
    if (c = this.classList) return c.remove(name);
    var c = this.className,
        cb = c.baseVal != null,
        cv = cb ? c.baseVal : c;
    cv = fnord3_collapse(cv.replace(re, " "));
    if (cb) c.baseVal = cv;
    else this.className = cv;
  }

  function classedFunction() {
    (value.apply(this, arguments)
        ? classedAdd
        : classedRemove).call(this);
  }

  return this.each(typeof value === "function"
      ? classedFunction : value
      ? classedAdd
      : classedRemove);
}
fnord3_selectionPrototype.style = function(name, value, priority) {
  if (arguments.length < 3) priority = "";

  // If no value is specified, return the first value.
  if (arguments.length < 2) return window
      .getComputedStyle(this.node(), null)
      .getPropertyValue(name);

  function styleNull() {
    this.style.removeProperty(name);
  }

  function styleConstant() {
    this.style.setProperty(name, value, priority);
  }

  function styleFunction() {
    var x = value.apply(this, arguments);
    if (x == null) this.style.removeProperty(name);
    else this.style.setProperty(name, x, priority);
  }

  return this.each(value == null
      ? styleNull : (typeof value === "function"
      ? styleFunction : styleConstant));
};
fnord3_selectionPrototype.property = function(name, value) {

  // If no value is specified, return the first value.
  if (arguments.length < 2) return this.node()[name];

  function propertyNull() {
    delete this[name];
  }

  function propertyConstant() {
    this[name] = value;
  }

  function propertyFunction() {
    var x = value.apply(this, arguments);
    if (x == null) delete this[name];
    else this[name] = x;
  }

  return this.each(value == null
      ? propertyNull : (typeof value === "function"
      ? propertyFunction : propertyConstant));
};
fnord3_selectionPrototype.text = function(value) {
  return arguments.length < 1
      ? this.node().textContent : this.each(typeof value === "function"
      ? function() { var v = value.apply(this, arguments); this.textContent = v == null ? "" : v; } : value == null
      ? function() { this.textContent = ""; }
      : function() { this.textContent = value; });
};
fnord3_selectionPrototype.html = function(value) {
  return arguments.length < 1
      ? this.node().innerHTML : this.each(typeof value === "function"
      ? function() { var v = value.apply(this, arguments); this.innerHTML = v == null ? "" : v; } : value == null
      ? function() { this.innerHTML = ""; }
      : function() { this.innerHTML = value; });
};
// TODO append(node)?
// TODO append(function)?
fnord3_selectionPrototype.append = function(name) {
  name = fnord3.ns.qualify(name);

  function append() {
    return this.appendChild(document.createElementNS(this.namespaceURI, name));
  }

  function appendNS() {
    return this.appendChild(document.createElementNS(name.space, name.local));
  }

  return this.select(name.local ? appendNS : append);
};
// TODO insert(node, function)?
// TODO insert(function, string)?
// TODO insert(function, function)?
fnord3_selectionPrototype.insert = function(name, before) {
  name = fnord3.ns.qualify(name);

  function insert() {
    return this.insertBefore(
        document.createElementNS(this.namespaceURI, name),
        fnord3_select(before, this));
  }

  function insertNS() {
    return this.insertBefore(
        document.createElementNS(name.space, name.local),
        fnord3_select(before, this));
  }

  return this.select(name.local ? insertNS : insert);
};
// TODO remove(selector)?
// TODO remove(node)?
// TODO remove(function)?
fnord3_selectionPrototype.remove = function() {
  return this.each(function() {
    var parent = this.parentNode;
    if (parent) parent.removeChild(this);
  });
};
fnord3_selectionPrototype.data = function(value, key) {
  var i = -1,
      n = this.length,
      group,
      node;

  // If no value is specified, return the first value.
  if (!arguments.length) {
    value = new Array(n = (group = this[0]).length);
    while (++i < n) {
      if (node = group[i]) {
        value[i] = node.__data__;
      }
    }
    return value;
  }

  function bind(group, groupData) {
    var i,
        n = group.length,
        m = groupData.length,
        n0 = Math.min(n, m),
        n1 = Math.max(n, m),
        updateNodes = [],
        enterNodes = [],
        exitNodes = [],
        node,
        nodeData;

    if (key) {
      var nodeByKeyValue = new fnord3_Map,
          keyValues = [],
          keyValue,
          j = groupData.length;

      for (i = -1; ++i < n;) {
        keyValue = key.call(node = group[i], node.__data__, i);
        if (nodeByKeyValue.has(keyValue)) {
          exitNodes[j++] = node; // duplicate key
        } else {
          nodeByKeyValue.set(keyValue, node);
        }
        keyValues.push(keyValue);
      }

      for (i = -1; ++i < m;) {
        keyValue = key.call(groupData, nodeData = groupData[i], i)
        if (nodeByKeyValue.has(keyValue)) {
          updateNodes[i] = node = nodeByKeyValue.get(keyValue);
          node.__data__ = nodeData;
          enterNodes[i] = exitNodes[i] = null;
        } else {
          enterNodes[i] = fnord3_selection_dataNode(nodeData);
          updateNodes[i] = exitNodes[i] = null;
        }
        nodeByKeyValue.remove(keyValue);
      }

      for (i = -1; ++i < n;) {
        if (nodeByKeyValue.has(keyValues[i])) {
          exitNodes[i] = group[i];
        }
      }
    } else {
      for (i = -1; ++i < n0;) {
        node = group[i];
        nodeData = groupData[i];
        if (node) {
          node.__data__ = nodeData;
          updateNodes[i] = node;
          enterNodes[i] = exitNodes[i] = null;
        } else {
          enterNodes[i] = fnord3_selection_dataNode(nodeData);
          updateNodes[i] = exitNodes[i] = null;
        }
      }
      for (; i < m; ++i) {
        enterNodes[i] = fnord3_selection_dataNode(groupData[i]);
        updateNodes[i] = exitNodes[i] = null;
      }
      for (; i < n1; ++i) {
        exitNodes[i] = group[i];
        enterNodes[i] = updateNodes[i] = null;
      }
    }

    enterNodes.update
        = updateNodes;

    enterNodes.parentNode
        = updateNodes.parentNode
        = exitNodes.parentNode
        = group.parentNode;

    enter.push(enterNodes);
    update.push(updateNodes);
    exit.push(exitNodes);
  }

  var enter = fnord3_selection_enter([]),
      update = fnord3_selection([]),
      exit = fnord3_selection([]);

  if (typeof value === "function") {
    while (++i < n) {
      bind(group = this[i], value.call(group, group.parentNode.__data__, i));
    }
  } else {
    while (++i < n) {
      bind(group = this[i], value);
    }
  }

  update.enter = function() { return enter; };
  update.exit = function() { return exit; };
  return update;
};

function fnord3_selection_dataNode(data) {
  return {__data__: data};
}
fnord3_selectionPrototype.datum =
fnord3_selectionPrototype.map = function(value) {
  return arguments.length < 1
      ? this.property("__data__")
      : this.property("__data__", value);
};
fnord3_selectionPrototype.filter = function(filter) {
  var subgroups = [],
      subgroup,
      group,
      node;

  if (typeof filter !== "function") filter = fnord3_selection_filter(filter);

  for (var j = 0, m = this.length; j < m; j++) {
    subgroups.push(subgroup = []);
    subgroup.parentNode = (group = this[j]).parentNode;
    for (var i = 0, n = group.length; i < n; i++) {
      if ((node = group[i]) && filter.call(node, node.__data__, i)) {
        subgroup.push(node);
      }
    }
  }

  return fnord3_selection(subgroups);
};

function fnord3_selection_filter(selector) {
  return function() {
    return fnord3_selectMatches(this, selector);
  };
}
fnord3_selectionPrototype.order = function() {
  for (var j = -1, m = this.length; ++j < m;) {
    for (var group = this[j], i = group.length - 1, next = group[i], node; --i >= 0;) {
      if (node = group[i]) {
        if (next && next !== node.nextSibling) next.parentNode.insertBefore(node, next);
        next = node;
      }
    }
  }
  return this;
};
fnord3_selectionPrototype.sort = function(comparator) {
  comparator = fnord3_selection_sortComparator.apply(this, arguments);
  for (var j = -1, m = this.length; ++j < m;) this[j].sort(comparator);
  return this.order();
};

function fnord3_selection_sortComparator(comparator) {
  if (!arguments.length) comparator = fnord3.ascending;
  return function(a, b) {
    return comparator(a && a.__data__, b && b.__data__);
  };
}
// type can be namespaced, e.g., "click.foo"
// listener can be null for removal
fnord3_selectionPrototype.on = function(type, listener, capture) {
  if (arguments.length < 3) capture = false;

  // parse the type specifier
  var name = "__on" + type, i = type.indexOf(".");
  if (i > 0) type = type.substring(0, i);

  // if called with only one argument, return the current listener
  if (arguments.length < 2) return (i = this.node()[name]) && i._;

  // remove the old event listener, and add the new event listener
  return this.each(function(d, i) {
    var node = this,
        o = node[name];

    // remove the old listener, if any (using the previously-set capture)
    if (o) {
      node.removeEventListener(type, o, o.$);
      delete node[name];
    }

    // add the new listener, if any (remembering the capture flag)
    if (listener) {
      node.addEventListener(type, node[name] = l, l.$ = capture);
      l._ = listener; // stash the unwrapped listener for get
    }

    // wrapped event listener that preserves i
    function l(e) {
      var o = fnord3.event; // Events can be reentrant (e.g., focus).
      fnord3.event = e;
      try {
        listener.call(node, node.__data__, i);
      } finally {
        fnord3.event = o;
      }
    }
  });
};
fnord3_selectionPrototype.each = function(callback) {
  for (var j = -1, m = this.length; ++j < m;) {
    for (var group = this[j], i = -1, n = group.length; ++i < n;) {
      var node = group[i];
      if (node) callback.call(node, node.__data__, i, j);
    }
  }
  return this;
};
//
// Note: assigning to the arguments array simultaneously changes the value of
// the corresponding argument!
//
// TODO The `this` argument probably shouldn't be the first argument to the
// callback, anyway, since it's redundant. However, that will require a major
// version bump due to backwards compatibility, so I'm not changing it right
// away.
//
fnord3_selectionPrototype.call = function(callback) {
  callback.apply(this, (arguments[0] = this, arguments));
  return this;
};
fnord3_selectionPrototype.empty = function() {
  return !this.node();
};
fnord3_selectionPrototype.node = function(callback) {
  for (var j = 0, m = this.length; j < m; j++) {
    for (var group = this[j], i = 0, n = group.length; i < n; i++) {
      var node = group[i];
      if (node) return node;
    }
  }
  return null;
};
fnord3_selectionPrototype.transition = function() {
  var subgroups = [],
      subgroup,
      node;

  for (var j = -1, m = this.length; ++j < m;) {
    subgroups.push(subgroup = []);
    for (var group = this[j], i = -1, n = group.length; ++i < n;) {
      subgroup.push((node = group[i]) ? {node: node, delay: fnord3_transitionDelay, duration: fnord3_transitionDuration} : null);
    }
  }

  return fnord3_transition(subgroups, fnord3_transitionId || ++fnord3_transitionNextId, Date.now());
};
var fnord3_selectionRoot = fnord3_selection([[document]]);

fnord3_selectionRoot[0].parentNode = fnord3_selectRoot;

// TODO fast singleton implementation!
// TODO select(function)
fnord3.select = function(selector) {
  return typeof selector === "string"
      ? fnord3_selectionRoot.select(selector)
      : fnord3_selection([[selector]]); // assume node
};

// TODO selectAll(function)
fnord3.selectAll = function(selector) {
  return typeof selector === "string"
      ? fnord3_selectionRoot.selectAll(selector)
      : fnord3_selection([fnord3_array(selector)]); // assume node[]
};
function fnord3_selection_enter(selection) {
  fnord3_arraySubclass(selection, fnord3_selection_enterPrototype);
  return selection;
}

var fnord3_selection_enterPrototype = [];

fnord3.selection.enter = fnord3_selection_enter;
fnord3.selection.enter.prototype = fnord3_selection_enterPrototype;

fnord3_selection_enterPrototype.append = fnord3_selectionPrototype.append;
fnord3_selection_enterPrototype.insert = fnord3_selectionPrototype.insert;
fnord3_selection_enterPrototype.empty = fnord3_selectionPrototype.empty;
fnord3_selection_enterPrototype.node = fnord3_selectionPrototype.node;
fnord3_selection_enterPrototype.select = function(selector) {
  var subgroups = [],
      subgroup,
      subnode,
      upgroup,
      group,
      node;

  for (var j = -1, m = this.length; ++j < m;) {
    upgroup = (group = this[j]).update;
    subgroups.push(subgroup = []);
    subgroup.parentNode = group.parentNode;
    for (var i = -1, n = group.length; ++i < n;) {
      if (node = group[i]) {
        subgroup.push(upgroup[i] = subnode = selector.call(group.parentNode, node.__data__, i));
        subnode.__data__ = node.__data__;
      } else {
        subgroup.push(null);
      }
    }
  }

  return fnord3_selection(subgroups);
};
function fnord3_transition(groups, id, time) {
  fnord3_arraySubclass(groups, fnord3_transitionPrototype);

  var tweens = new fnord3_Map,
      event = fnord3.dispatch("start", "end"),
      ease = fnord3_transitionEase;

  groups.id = id;

  groups.time = time;

  groups.tween = function(name, tween) {
    if (arguments.length < 2) return tweens.get(name);
    if (tween == null) tweens.remove(name);
    else tweens.set(name, tween);
    return groups;
  };

  groups.ease = function(value) {
    if (!arguments.length) return ease;
    ease = typeof value === "function" ? value : fnord3.ease.apply(fnord3, arguments);
    return groups;
  };

  groups.each = function(type, listener) {
    if (arguments.length < 2) return fnord3_transition_each.call(groups, type);
    event.on(type, listener);
    return groups;
  };

  fnord3.timer(function(elapsed) {
    groups.each(function(d, i, j) {
      var tweened = [],
          node = this,
          delay = groups[j][i].delay,
          duration = groups[j][i].duration,
          lock = node.__transition__ || (node.__transition__ = {active: 0, count: 0});

      ++lock.count;

      delay <= elapsed ? start(elapsed) : fnord3.timer(start, delay, time);

      function start(elapsed) {
        if (lock.active > id) return stop();
        lock.active = id;

        tweens.forEach(function(key, value) {
          if (value = value.call(node, d, i)) {
            tweened.push(value);
          }
        });

        event.start.call(node, d, i);
        if (!tick(elapsed)) fnord3.timer(tick, 0, time);
        return 1;
      }

      function tick(elapsed) {
        if (lock.active !== id) return stop();

        var t = (elapsed - delay) / duration,
            e = ease(t),
            n = tweened.length;

        while (n > 0) {
          tweened[--n].call(node, e);
        }

        if (t >= 1) {
          stop();
          fnord3_transitionId = id;
          event.end.call(node, d, i);
          fnord3_transitionId = 0;
          return 1;
        }
      }

      function stop() {
        if (!--lock.count) delete node.__transition__;
        return 1;
      }
    });
    return 1;
  }, 0, time);

  return groups;
}

var fnord3_transitionRemove = {};

function fnord3_transitionNull(d, i, a) {
  return a != "" && fnord3_transitionRemove;
}

function fnord3_transitionTween(name, b) {
  var interpolate = fnord3_interpolateByName(name);

  function transitionFunction(d, i, a) {
    var v = b.call(this, d, i);
    return v == null
        ? a != "" && fnord3_transitionRemove
        : a != v && interpolate(a, v);
  }

  function transitionString(d, i, a) {
    return a != b && interpolate(a, b);
  }

  return typeof b === "function" ? transitionFunction
      : b == null ? fnord3_transitionNull
      : (b += "", transitionString);
}

var fnord3_transitionPrototype = [],
    fnord3_transitionNextId = 0,
    fnord3_transitionId = 0,
    fnord3_transitionDefaultDelay = 0,
    fnord3_transitionDefaultDuration = 250,
    fnord3_transitionDefaultEase = fnord3.ease("cubic-in-out"),
    fnord3_transitionDelay = fnord3_transitionDefaultDelay,
    fnord3_transitionDuration = fnord3_transitionDefaultDuration,
    fnord3_transitionEase = fnord3_transitionDefaultEase;

fnord3_transitionPrototype.call = fnord3_selectionPrototype.call;

fnord3.transition = function(selection) {
  return arguments.length
      ? (fnord3_transitionId ? selection.transition() : selection)
      : fnord3_selectionRoot.transition();
};

fnord3.transition.prototype = fnord3_transitionPrototype;
fnord3_transitionPrototype.select = function(selector) {
  var subgroups = [],
      subgroup,
      subnode,
      node;

  if (typeof selector !== "function") selector = fnord3_selection_selector(selector);

  for (var j = -1, m = this.length; ++j < m;) {
    subgroups.push(subgroup = []);
    for (var group = this[j], i = -1, n = group.length; ++i < n;) {
      if ((node = group[i]) && (subnode = selector.call(node.node, node.node.__data__, i))) {
        if ("__data__" in node.node) subnode.__data__ = node.node.__data__;
        subgroup.push({node: subnode, delay: node.delay, duration: node.duration});
      } else {
        subgroup.push(null);
      }
    }
  }

  return fnord3_transition(subgroups, this.id, this.time).ease(this.ease());
};
fnord3_transitionPrototype.selectAll = function(selector) {
  var subgroups = [],
      subgroup,
      subnodes,
      node;

  if (typeof selector !== "function") selector = fnord3_selection_selectorAll(selector);

  for (var j = -1, m = this.length; ++j < m;) {
    for (var group = this[j], i = -1, n = group.length; ++i < n;) {
      if (node = group[i]) {
        subnodes = selector.call(node.node, node.node.__data__, i);
        subgroups.push(subgroup = []);
        for (var k = -1, o = subnodes.length; ++k < o;) {
          subgroup.push({node: subnodes[k], delay: node.delay, duration: node.duration});
        }
      }
    }
  }

  return fnord3_transition(subgroups, this.id, this.time).ease(this.ease());
};
fnord3_transitionPrototype.attr = function(name, value) {
  return this.attrTween(name, fnord3_transitionTween(name, value));
};

fnord3_transitionPrototype.attrTween = function(nameNS, tween) {
  var name = fnord3.ns.qualify(nameNS);

  function attrTween(d, i) {
    var f = tween.call(this, d, i, this.getAttribute(name));
    return f === fnord3_transitionRemove
        ? (this.removeAttribute(name), null)
        : f && function(t) { this.setAttribute(name, f(t)); };
  }

  function attrTweenNS(d, i) {
    var f = tween.call(this, d, i, this.getAttributeNS(name.space, name.local));
    return f === fnord3_transitionRemove
        ? (this.removeAttributeNS(name.space, name.local), null)
        : f && function(t) { this.setAttributeNS(name.space, name.local, f(t)); };
  }

  return this.tween("attr." + nameNS, name.local ? attrTweenNS : attrTween);
};
fnord3_transitionPrototype.style = function(name, value, priority) {
  if (arguments.length < 3) priority = "";
  return this.styleTween(name, fnord3_transitionTween(name, value), priority);
};

fnord3_transitionPrototype.styleTween = function(name, tween, priority) {
  if (arguments.length < 3) priority = "";
  return this.tween("style." + name, function(d, i) {
    var f = tween.call(this, d, i, window.getComputedStyle(this, null).getPropertyValue(name));
    return f === fnord3_transitionRemove
        ? (this.style.removeProperty(name), null)
        : f && function(t) { this.style.setProperty(name, f(t), priority); };
  });
};
fnord3_transitionPrototype.text = function(value) {
  return this.tween("text", function(d, i) {
    this.textContent = typeof value === "function"
        ? value.call(this, d, i)
        : value;
  });
};
fnord3_transitionPrototype.remove = function() {
  return this.each("end.transition", function() {
    var p;
    if (!this.__transition__ && (p = this.parentNode)) p.removeChild(this);
  });
};
fnord3_transitionPrototype.delay = function(value) {
  var groups = this;
  return groups.each(typeof value === "function"
      ? function(d, i, j) { groups[j][i].delay = value.apply(this, arguments) | 0; }
      : (value = value | 0, function(d, i, j) { groups[j][i].delay = value; }));
};
fnord3_transitionPrototype.duration = function(value) {
  var groups = this;
  return groups.each(typeof value === "function"
      ? function(d, i, j) { groups[j][i].duration = Math.max(1, value.apply(this, arguments) | 0); }
      : (value = Math.max(1, value | 0), function(d, i, j) { groups[j][i].duration = value; }));
};
function fnord3_transition_each(callback) {
  var id = fnord3_transitionId,
      ease = fnord3_transitionEase,
      delay = fnord3_transitionDelay,
      duration = fnord3_transitionDuration;

  fnord3_transitionId = this.id;
  fnord3_transitionEase = this.ease();
  for (var j = 0, m = this.length; j < m; j++) {
    for (var group = this[j], i = 0, n = group.length; i < n; i++) {
      var node = group[i];
      if (node) {
        fnord3_transitionDelay = this[j][i].delay;
        fnord3_transitionDuration = this[j][i].duration;
        callback.call(node = node.node, node.__data__, i, j);
      }
    }
  }

  fnord3_transitionId = id;
  fnord3_transitionEase = ease;
  fnord3_transitionDelay = delay;
  fnord3_transitionDuration = duration;
  return this;
}
fnord3_transitionPrototype.transition = function() {
  return this.select(fnord3_this);
};
var fnord3_timer_queue = null,
    fnord3_timer_interval, // is an interval (or frame) active?
    fnord3_timer_timeout; // is a timeout active?

// The timer will continue to fire until callback returns true.
fnord3.timer = function(callback, delay, then) {
  var found = false,
      t0,
      t1 = fnord3_timer_queue;

  if (arguments.length < 3) {
    if (arguments.length < 2) delay = 0;
    else if (!isFinite(delay)) return;
    then = Date.now();
  }

  // See if the callback's already in the queue.
  while (t1) {
    if (t1.callback === callback) {
      t1.then = then;
      t1.delay = delay;
      found = true;
      break;
    }
    t0 = t1;
    t1 = t1.next;
  }

  // Otherwise, add the callback to the queue.
  if (!found) fnord3_timer_queue = {
    callback: callback,
    then: then,
    delay: delay,
    next: fnord3_timer_queue
  };

  // Start animatin'!
  if (!fnord3_timer_interval) {
    fnord3_timer_timeout = clearTimeout(fnord3_timer_timeout);
    fnord3_timer_interval = 1;
    fnord3_timer_frame(fnord3_timer_step);
  }
}

function fnord3_timer_step() {
  var elapsed,
      now = Date.now(),
      t1 = fnord3_timer_queue;

  while (t1) {
    elapsed = now - t1.then;
    if (elapsed >= t1.delay) t1.flush = t1.callback(elapsed);
    t1 = t1.next;
  }

  var delay = fnord3_timer_flush() - now;
  if (delay > 24) {
    if (isFinite(delay)) {
      clearTimeout(fnord3_timer_timeout);
      fnord3_timer_timeout = setTimeout(fnord3_timer_step, delay);
    }
    fnord3_timer_interval = 0;
  } else {
    fnord3_timer_interval = 1;
    fnord3_timer_frame(fnord3_timer_step);
  }
}

fnord3.timer.flush = function() {
  var elapsed,
      now = Date.now(),
      t1 = fnord3_timer_queue;

  while (t1) {
    elapsed = now - t1.then;
    if (!t1.delay) t1.flush = t1.callback(elapsed);
    t1 = t1.next;
  }

  fnord3_timer_flush();
};

// Flush after callbacks, to avoid concurrent queue modification.
function fnord3_timer_flush() {
  var t0 = null,
      t1 = fnord3_timer_queue,
      then = Infinity;
  while (t1) {
    if (t1.flush) {
      t1 = t0 ? t0.next = t1.next : fnord3_timer_queue = t1.next;
    } else {
      then = Math.min(then, t1.then + t1.delay);
      t1 = (t0 = t1).next;
    }
  }
  return then;
}

var fnord3_timer_frame = window.requestAnimationFrame
    || window.webkitRequestAnimationFrame
    || window.mozRequestAnimationFrame
    || window.oRequestAnimationFrame
    || window.msRequestAnimationFrame
    || function(callback) { setTimeout(callback, 17); };
fnord3.transform = function(string) {
  var g = document.createElementNS(fnord3.ns.prefix.svg, "g"),
      identity = {a: 1, b: 0, c: 0, d: 1, e: 0, f: 0};
  return (fnord3.transform = function(string) {
    g.setAttribute("transform", string);
    var t = g.transform.baseVal.consolidate();
    return new fnord3_transform(t ? t.matrix : identity);
  })(string);
};

// Compute x-scale and normalize the first row.
// Compute shear and make second row orthogonal to first.
// Compute y-scale and normalize the second row.
// Finally, compute the rotation.
function fnord3_transform(m) {
  var r0 = [m.a, m.b],
      r1 = [m.c, m.d],
      kx = fnord3_transformNormalize(r0),
      kz = fnord3_transformDot(r0, r1),
      ky = fnord3_transformNormalize(fnord3_transformCombine(r1, r0, -kz)) || 0;
  if (r0[0] * r1[1] < r1[0] * r0[1]) {
    r0[0] *= -1;
    r0[1] *= -1;
    kx *= -1;
    kz *= -1;
  }
  this.rotate = (kx ? Math.atan2(r0[1], r0[0]) : Math.atan2(-r1[0], r1[1])) * fnord3_transformDegrees;
  this.translate = [m.e, m.f];
  this.scale = [kx, ky];
  this.skew = ky ? Math.atan2(kz, ky) * fnord3_transformDegrees : 0;
};

fnord3_transform.prototype.toString = function() {
  return "translate(" + this.translate
      + ")rotate(" + this.rotate
      + ")skewX(" + this.skew
      + ")scale(" + this.scale
      + ")";
};

function fnord3_transformDot(a, b) {
  return a[0] * b[0] + a[1] * b[1];
}

function fnord3_transformNormalize(a) {
  var k = Math.sqrt(fnord3_transformDot(a, a));
  if (k) {
    a[0] /= k;
    a[1] /= k;
  }
  return k;
}

function fnord3_transformCombine(a, b, k) {
  a[0] += k * b[0];
  a[1] += k * b[1];
  return a;
}

var fnord3_transformDegrees = 180 / Math.PI;
fnord3.mouse = function(container) {
  return fnord3_mousePoint(container, fnord3_eventSource());
};

// https://bugs.webkit.org/show_bug.cgi?id=44083
var fnord3_mouse_bug44083 = /WebKit/.test(navigator.userAgent) ? -1 : 0;

function fnord3_mousePoint(container, e) {
  var svg = container.ownerSVGElement || container;
  if (svg.createSVGPoint) {
    var point = svg.createSVGPoint();
    if ((fnord3_mouse_bug44083 < 0) && (window.scrollX || window.scrollY)) {
      svg = fnord3.select(document.body)
        .append("svg")
          .style("position", "absolute")
          .style("top", 0)
          .style("left", 0);
      var ctm = svg[0][0].getScreenCTM();
      fnord3_mouse_bug44083 = !(ctm.f || ctm.e);
      svg.remove();
    }
    if (fnord3_mouse_bug44083) {
      point.x = e.pageX;
      point.y = e.pageY;
    } else {
      point.x = e.clientX;
      point.y = e.clientY;
    }
    point = point.matrixTransform(container.getScreenCTM().inverse());
    return [point.x, point.y];
  }
  var rect = container.getBoundingClientRect();
  return [e.clientX - rect.left - container.clientLeft, e.clientY - rect.top - container.clientTop];
};
fnord3.touches = function(container, touches) {
  if (arguments.length < 2) touches = fnord3_eventSource().touches;
  return touches ? fnord3_array(touches).map(function(touch) {
    var point = fnord3_mousePoint(container, touch);
    point.identifier = touch.identifier;
    return point;
  }) : [];
};
function fnord3_noop() {}
fnord3.scale = {};

function fnord3_scaleExtent(domain) {
  var start = domain[0], stop = domain[domain.length - 1];
  return start < stop ? [start, stop] : [stop, start];
}

function fnord3_scaleRange(scale) {
  return scale.rangeExtent ? scale.rangeExtent() : fnord3_scaleExtent(scale.range());
}
function fnord3_scale_nice(domain, nice) {
  var i0 = 0,
      i1 = domain.length - 1,
      x0 = domain[i0],
      x1 = domain[i1],
      dx;

  if (x1 < x0) {
    dx = i0; i0 = i1; i1 = dx;
    dx = x0; x0 = x1; x1 = dx;
  }

  if (dx = x1 - x0) {
    nice = nice(dx);
    domain[i0] = nice.floor(x0);
    domain[i1] = nice.ceil(x1);
  }

  return domain;
}

function fnord3_scale_niceDefault() {
  return Math;
}
fnord3.scale.linear = function() {
  return fnord3_scale_linear([0, 1], [0, 1], fnord3.interpolate, false);
};

function fnord3_scale_linear(domain, range, interpolate, clamp) {
  var output,
      input;

  function rescale() {
    var linear = Math.min(domain.length, range.length) > 2 ? fnord3_scale_polylinear : fnord3_scale_bilinear,
        uninterpolate = clamp ? fnord3_uninterpolateClamp : fnord3_uninterpolateNumber;
    output = linear(domain, range, uninterpolate, interpolate);
    input = linear(range, domain, uninterpolate, fnord3.interpolate);
    return scale;
  }

  function scale(x) {
    return output(x);
  }

  // Note: requires range is coercible to number!
  scale.invert = function(y) {
    return input(y);
  };

  scale.domain = function(x) {
    if (!arguments.length) return domain;
    domain = x.map(Number);
    return rescale();
  };

  scale.range = function(x) {
    if (!arguments.length) return range;
    range = x;
    return rescale();
  };

  scale.rangeRound = function(x) {
    return scale.range(x).interpolate(fnord3.interpolateRound);
  };

  scale.clamp = function(x) {
    if (!arguments.length) return clamp;
    clamp = x;
    return rescale();
  };

  scale.interpolate = function(x) {
    if (!arguments.length) return interpolate;
    interpolate = x;
    return rescale();
  };

  scale.ticks = function(m) {
    return fnord3_scale_linearTicks(domain, m);
  };

  scale.tickFormat = function(m) {
    return fnord3_scale_linearTickFormat(domain, m);
  };

  scale.nice = function() {
    fnord3_scale_nice(domain, fnord3_scale_linearNice);
    return rescale();
  };

  scale.copy = function() {
    return fnord3_scale_linear(domain, range, interpolate, clamp);
  };

  return rescale();
}

function fnord3_scale_linearRebind(scale, linear) {
  return fnord3.rebind(scale, linear, "range", "rangeRound", "interpolate", "clamp");
}

function fnord3_scale_linearNice(dx) {
  dx = Math.pow(10, Math.round(Math.log(dx) / Math.LN10) - 1);
  return {
    floor: function(x) { return Math.floor(x / dx) * dx; },
    ceil: function(x) { return Math.ceil(x / dx) * dx; }
  };
}

function fnord3_scale_linearTickRange(domain, m) {
  var extent = fnord3_scaleExtent(domain),
      span = extent[1] - extent[0],
      step = Math.pow(10, Math.floor(Math.log(span / m) / Math.LN10)),
      err = m / span * step;

  // Filter ticks to get closer to the desired count.
  if (err <= .15) step *= 10;
  else if (err <= .35) step *= 5;
  else if (err <= .75) step *= 2;

  // Round start and stop values to step interval.
  extent[0] = Math.ceil(extent[0] / step) * step;
  extent[1] = Math.floor(extent[1] / step) * step + step * .5; // inclusive
  extent[2] = step;
  return extent;
}

function fnord3_scale_linearTicks(domain, m) {
  return fnord3.range.apply(fnord3, fnord3_scale_linearTickRange(domain, m));
}

function fnord3_scale_linearTickFormat(domain, m) {
  return fnord3.format(",." + Math.max(0, -Math.floor(Math.log(fnord3_scale_linearTickRange(domain, m)[2]) / Math.LN10 + .01)) + "f");
}
function fnord3_scale_bilinear(domain, range, uninterpolate, interpolate) {
  var u = uninterpolate(domain[0], domain[1]),
      i = interpolate(range[0], range[1]);
  return function(x) {
    return i(u(x));
  };
}
function fnord3_scale_polylinear(domain, range, uninterpolate, interpolate) {
  var u = [],
      i = [],
      j = 0,
      k = Math.min(domain.length, range.length) - 1;

  // Handle descending domains.
  if (domain[k] < domain[0]) {
    domain = domain.slice().reverse();
    range = range.slice().reverse();
  }

  while (++j <= k) {
    u.push(uninterpolate(domain[j - 1], domain[j]));
    i.push(interpolate(range[j - 1], range[j]));
  }

  return function(x) {
    var j = fnord3.bisect(domain, x, 1, k) - 1;
    return i[j](u[j](x));
  };
}
fnord3.scale.log = function() {
  return fnord3_scale_log(fnord3.scale.linear(), fnord3_scale_logp);
};

function fnord3_scale_log(linear, log) {
  var pow = log.pow;

  function scale(x) {
    return linear(log(x));
  }

  scale.invert = function(x) {
    return pow(linear.invert(x));
  };

  scale.domain = function(x) {
    if (!arguments.length) return linear.domain().map(pow);
    log = x[0] < 0 ? fnord3_scale_logn : fnord3_scale_logp;
    pow = log.pow;
    linear.domain(x.map(log));
    return scale;
  };

  scale.nice = function() {
    linear.domain(fnord3_scale_nice(linear.domain(), fnord3_scale_niceDefault));
    return scale;
  };

  scale.ticks = function() {
    var extent = fnord3_scaleExtent(linear.domain()),
        ticks = [];
    if (extent.every(isFinite)) {
      var i = Math.floor(extent[0]),
          j = Math.ceil(extent[1]),
          u = pow(extent[0]),
          v = pow(extent[1]);
      if (log === fnord3_scale_logn) {
        ticks.push(pow(i));
        for (; i++ < j;) for (var k = 9; k > 0; k--) ticks.push(pow(i) * k);
      } else {
        for (; i < j; i++) for (var k = 1; k < 10; k++) ticks.push(pow(i) * k);
        ticks.push(pow(i));
      }
      for (i = 0; ticks[i] < u; i++) {} // strip small values
      for (j = ticks.length; ticks[j - 1] > v; j--) {} // strip big values
      ticks = ticks.slice(i, j);
    }
    return ticks;
  };

  scale.tickFormat = function(n, format) {
    if (arguments.length < 2) format = fnord3_scale_logFormat;
    if (arguments.length < 1) return format;
    var k = n / scale.ticks().length,
        f = log === fnord3_scale_logn ? (e = -1e-12, Math.floor) : (e = 1e-12, Math.ceil),
        e;
    return function(d) {
      return d / pow(f(log(d) + e)) < k ? format(d) : "";
    };
  };

  scale.copy = function() {
    return fnord3_scale_log(linear.copy(), log);
  };

  return fnord3_scale_linearRebind(scale, linear);
}

var fnord3_scale_logFormat = fnord3.format(".0e");

function fnord3_scale_logp(x) {
  return Math.log(x < 0 ? 0 : x) / Math.LN10;
}

function fnord3_scale_logn(x) {
  return -Math.log(x > 0 ? 0 : -x) / Math.LN10;
}

fnord3_scale_logp.pow = function(x) {
  return Math.pow(10, x);
};

fnord3_scale_logn.pow = function(x) {
  return -Math.pow(10, -x);
};
fnord3.scale.pow = function() {
  return fnord3_scale_pow(fnord3.scale.linear(), 1);
};

function fnord3_scale_pow(linear, exponent) {
  var powp = fnord3_scale_powPow(exponent),
      powb = fnord3_scale_powPow(1 / exponent);

  function scale(x) {
    return linear(powp(x));
  }

  scale.invert = function(x) {
    return powb(linear.invert(x));
  };

  scale.domain = function(x) {
    if (!arguments.length) return linear.domain().map(powb);
    linear.domain(x.map(powp));
    return scale;
  };

  scale.ticks = function(m) {
    return fnord3_scale_linearTicks(scale.domain(), m);
  };

  scale.tickFormat = function(m) {
    return fnord3_scale_linearTickFormat(scale.domain(), m);
  };

  scale.nice = function() {
    return scale.domain(fnord3_scale_nice(scale.domain(), fnord3_scale_linearNice));
  };

  scale.exponent = function(x) {
    if (!arguments.length) return exponent;
    var domain = scale.domain();
    powp = fnord3_scale_powPow(exponent = x);
    powb = fnord3_scale_powPow(1 / exponent);
    return scale.domain(domain);
  };

  scale.copy = function() {
    return fnord3_scale_pow(linear.copy(), exponent);
  };

  return fnord3_scale_linearRebind(scale, linear);
}

function fnord3_scale_powPow(e) {
  return function(x) {
    return x < 0 ? -Math.pow(-x, e) : Math.pow(x, e);
  };
}
fnord3.scale.sqrt = function() {
  return fnord3.scale.pow().exponent(.5);
};
fnord3.scale.ordinal = function() {
  return fnord3_scale_ordinal([], {t: "range", x: []});
};

function fnord3_scale_ordinal(domain, ranger) {
  var index,
      range,
      rangeBand;

  function scale(x) {
    return range[((index.get(x) || index.set(x, domain.push(x))) - 1) % range.length];
  }

  function steps(start, step) {
    return fnord3.range(domain.length).map(function(i) { return start + step * i; });
  }

  scale.domain = function(x) {
    if (!arguments.length) return domain;
    domain = [];
    index = new fnord3_Map;
    var i = -1, n = x.length, xi;
    while (++i < n) if (!index.has(xi = x[i])) index.set(xi, domain.push(xi));
    return scale[ranger.t](ranger.x, ranger.p);
  };

  scale.range = function(x) {
    if (!arguments.length) return range;
    range = x;
    rangeBand = 0;
    ranger = {t: "range", x: x};
    return scale;
  };

  scale.rangePoints = function(x, padding) {
    if (arguments.length < 2) padding = 0;
    var start = x[0],
        stop = x[1],
        step = (stop - start) / (domain.length - 1 + padding);
    range = steps(domain.length < 2 ? (start + stop) / 2 : start + step * padding / 2, step);
    rangeBand = 0;
    ranger = {t: "rangePoints", x: x, p: padding};
    return scale;
  };

  scale.rangeBands = function(x, padding) {
    if (arguments.length < 2) padding = 0;
    var reverse = x[1] < x[0],
        start = x[reverse - 0],
        stop = x[1 - reverse],
        step = (stop - start) / (domain.length + padding);
    range = steps(start + step * padding, step);
    if (reverse) range.reverse();
    rangeBand = step * (1 - padding);
    ranger = {t: "rangeBands", x: x, p: padding};
    return scale;
  };

  scale.rangeRoundBands = function(x, padding) {
    if (arguments.length < 2) padding = 0;
    var reverse = x[1] < x[0],
        start = x[reverse - 0],
        stop = x[1 - reverse],
        step = Math.floor((stop - start) / (domain.length + padding)),
        error = stop - start - (domain.length - padding) * step;
    range = steps(start + Math.round(error / 2), step);
    if (reverse) range.reverse();
    rangeBand = Math.round(step * (1 - padding));
    ranger = {t: "rangeRoundBands", x: x, p: padding};
    return scale;
  };

  scale.rangeBand = function() {
    return rangeBand;
  };

  scale.rangeExtent = function() {
    return fnord3_scaleExtent(ranger.x);
  };

  scale.copy = function() {
    return fnord3_scale_ordinal(domain, ranger);
  };

  return scale.domain(domain);
}
/*
 * This product includes color specifications and designs developed by Cynthia
 * Brewer (http://colorbrewer.org/). See lib/colorbrewer for more information.
 */

fnord3.scale.category10 = function() {
  return fnord3.scale.ordinal().range(fnord3_category10);
};

fnord3.scale.category20 = function() {
  return fnord3.scale.ordinal().range(fnord3_category20);
};

fnord3.scale.category20b = function() {
  return fnord3.scale.ordinal().range(fnord3_category20b);
};

fnord3.scale.category20c = function() {
  return fnord3.scale.ordinal().range(fnord3_category20c);
};

var fnord3_category10 = [
  "#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd",
  "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf"
];

var fnord3_category20 = [
  "#1f77b4", "#aec7e8",
  "#ff7f0e", "#ffbb78",
  "#2ca02c", "#98df8a",
  "#d62728", "#ff9896",
  "#9467bd", "#c5b0d5",
  "#8c564b", "#c49c94",
  "#e377c2", "#f7b6d2",
  "#7f7f7f", "#c7c7c7",
  "#bcbd22", "#dbdb8d",
  "#17becf", "#9edae5"
];

var fnord3_category20b = [
  "#393b79", "#5254a3", "#6b6ecf", "#9c9ede",
  "#637939", "#8ca252", "#b5cf6b", "#cedb9c",
  "#8c6fnord31", "#bd9e39", "#e7ba52", "#e7cb94",
  "#843c39", "#ad494a", "#d6616b", "#e7969c",
  "#7b4173", "#a55194", "#ce6dbd", "#de9ed6"
];

var fnord3_category20c = [
  "#3182bd", "#6baed6", "#9ecae1", "#c6dbef",
  "#e6550d", "#fd8fnord3c", "#fdae6b", "#fdd0a2",
  "#31a354", "#74c476", "#a1d99b", "#c7e9c0",
  "#756bb1", "#9e9ac8", "#bcbddc", "#dadaeb",
  "#636363", "#969696", "#bdbdbd", "#d9d9d9"
];
fnord3.scale.quantile = function() {
  return fnord3_scale_quantile([], []);
};

function fnord3_scale_quantile(domain, range) {
  var thresholds;

  function rescale() {
    var k = 0,
        n = domain.length,
        q = range.length;
    thresholds = [];
    while (++k < q) thresholds[k - 1] = fnord3.quantile(domain, k / q);
    return scale;
  }

  function scale(x) {
    if (isNaN(x = +x)) return NaN;
    return range[fnord3.bisect(thresholds, x)];
  }

  scale.domain = function(x) {
    if (!arguments.length) return domain;
    domain = x.filter(function(d) { return !isNaN(d); }).sort(fnord3.ascending);
    return rescale();
  };

  scale.range = function(x) {
    if (!arguments.length) return range;
    range = x;
    return rescale();
  };

  scale.quantiles = function() {
    return thresholds;
  };

  scale.copy = function() {
    return fnord3_scale_quantile(domain, range); // copy on write!
  };

  return rescale();
}
fnord3.scale.quantize = function() {
  return fnord3_scale_quantize(0, 1, [0, 1]);
};

function fnord3_scale_quantize(x0, x1, range) {
  var kx, i;

  function scale(x) {
    return range[Math.max(0, Math.min(i, Math.floor(kx * (x - x0))))];
  }

  function rescale() {
    kx = range.length / (x1 - x0);
    i = range.length - 1;
    return scale;
  }

  scale.domain = function(x) {
    if (!arguments.length) return [x0, x1];
    x0 = +x[0];
    x1 = +x[x.length - 1];
    return rescale();
  };

  scale.range = function(x) {
    if (!arguments.length) return range;
    range = x;
    return rescale();
  };

  scale.copy = function() {
    return fnord3_scale_quantize(x0, x1, range); // copy on write
  };

  return rescale();
}
fnord3.scale.identity = function() {
  return fnord3_scale_identity([0, 1]);
};

function fnord3_scale_identity(domain) {

  function identity(x) { return +x; }

  identity.invert = identity;

  identity.domain = identity.range = function(x) {
    if (!arguments.length) return domain;
    domain = x.map(identity);
    return identity;
  };

  identity.ticks = function(m) {
    return fnord3_scale_linearTicks(domain, m);
  };

  identity.tickFormat = function(m) {
    return fnord3_scale_linearTickFormat(domain, m);
  };

  identity.copy = function() {
    return fnord3_scale_identity(domain);
  };

  return identity;
}
fnord3.svg = {};
fnord3.svg.arc = function() {
  var innerRadius = fnord3_svg_arcInnerRadius,
      outerRadius = fnord3_svg_arcOuterRadius,
      startAngle = fnord3_svg_arcStartAngle,
      endAngle = fnord3_svg_arcEndAngle;

  function arc() {
    var r0 = innerRadius.apply(this, arguments),
        r1 = outerRadius.apply(this, arguments),
        a0 = startAngle.apply(this, arguments) + fnord3_svg_arcOffset,
        a1 = endAngle.apply(this, arguments) + fnord3_svg_arcOffset,
        da = (a1 < a0 && (da = a0, a0 = a1, a1 = da), a1 - a0),
        df = da < Math.PI ? "0" : "1",
        c0 = Math.cos(a0),
        s0 = Math.sin(a0),
        c1 = Math.cos(a1),
        s1 = Math.sin(a1);
    return da >= fnord3_svg_arcMax
      ? (r0
      ? "M0," + r1
      + "A" + r1 + "," + r1 + " 0 1,1 0," + (-r1)
      + "A" + r1 + "," + r1 + " 0 1,1 0," + r1
      + "M0," + r0
      + "A" + r0 + "," + r0 + " 0 1,0 0," + (-r0)
      + "A" + r0 + "," + r0 + " 0 1,0 0," + r0
      + "Z"
      : "M0," + r1
      + "A" + r1 + "," + r1 + " 0 1,1 0," + (-r1)
      + "A" + r1 + "," + r1 + " 0 1,1 0," + r1
      + "Z")
      : (r0
      ? "M" + r1 * c0 + "," + r1 * s0
      + "A" + r1 + "," + r1 + " 0 " + df + ",1 " + r1 * c1 + "," + r1 * s1
      + "L" + r0 * c1 + "," + r0 * s1
      + "A" + r0 + "," + r0 + " 0 " + df + ",0 " + r0 * c0 + "," + r0 * s0
      + "Z"
      : "M" + r1 * c0 + "," + r1 * s0
      + "A" + r1 + "," + r1 + " 0 " + df + ",1 " + r1 * c1 + "," + r1 * s1
      + "L0,0"
      + "Z");
  }

  arc.innerRadius = function(v) {
    if (!arguments.length) return innerRadius;
    innerRadius = fnord3_functor(v);
    return arc;
  };

  arc.outerRadius = function(v) {
    if (!arguments.length) return outerRadius;
    outerRadius = fnord3_functor(v);
    return arc;
  };

  arc.startAngle = function(v) {
    if (!arguments.length) return startAngle;
    startAngle = fnord3_functor(v);
    return arc;
  };

  arc.endAngle = function(v) {
    if (!arguments.length) return endAngle;
    endAngle = fnord3_functor(v);
    return arc;
  };

  arc.centroid = function() {
    var r = (innerRadius.apply(this, arguments)
        + outerRadius.apply(this, arguments)) / 2,
        a = (startAngle.apply(this, arguments)
        + endAngle.apply(this, arguments)) / 2 + fnord3_svg_arcOffset;
    return [Math.cos(a) * r, Math.sin(a) * r];
  };

  return arc;
};

var fnord3_svg_arcOffset = -Math.PI / 2,
    fnord3_svg_arcMax = 2 * Math.PI - 1e-6;

function fnord3_svg_arcInnerRadius(d) {
  return d.innerRadius;
}

function fnord3_svg_arcOuterRadius(d) {
  return d.outerRadius;
}

function fnord3_svg_arcStartAngle(d) {
  return d.startAngle;
}

function fnord3_svg_arcEndAngle(d) {
  return d.endAngle;
}
function fnord3_svg_line(projection) {
  var x = fnord3_svg_lineX,
      y = fnord3_svg_lineY,
      defined = fnord3_true,
      interpolate = fnord3_svg_lineInterpolatorDefault,
      interpolator = fnord3_svg_lineLinear,
      tension = .7;

  function line(data) {
    var segments = [],
        points = [],
        i = -1,
        n = data.length,
        d,
        fx = fnord3_functor(x),
        fy = fnord3_functor(y);

    function segment() {
      segments.push("M", interpolator(projection(points), tension));
    }

    while (++i < n) {
      if (defined.call(this, d = data[i], i)) {
        points.push([+fx.call(this, d, i), +fy.call(this, d, i)]);
      } else if (points.length) {
        segment();
        points = [];
      }
    }

    if (points.length) segment();

    return segments.length ? segments.join("") : null;
  }

  line.x = function(_) {
    if (!arguments.length) return x;
    x = _;
    return line;
  };

  line.y = function(_) {
    if (!arguments.length) return y;
    y = _;
    return line;
  };

  line.defined  = function(_) {
    if (!arguments.length) return defined;
    defined = _;
    return line;
  };

  line.interpolate = function(_) {
    if (!arguments.length) return interpolate;
    if (!fnord3_svg_lineInterpolators.has(_ += "")) _ = fnord3_svg_lineInterpolatorDefault;
    interpolator = fnord3_svg_lineInterpolators.get(interpolate = _);
    return line;
  };

  line.tension = function(_) {
    if (!arguments.length) return tension;
    tension = _;
    return line;
  };

  return line;
}

fnord3.svg.line = function() {
  return fnord3_svg_line(fnord3_identity);
};

// The default `x` property, which references d[0].
function fnord3_svg_lineX(d) {
  return d[0];
}

// The default `y` property, which references d[1].
function fnord3_svg_lineY(d) {
  return d[1];
}

var fnord3_svg_lineInterpolatorDefault = "linear";

// The various interpolators supported by the `line` class.
var fnord3_svg_lineInterpolators = fnord3.map({
  "linear": fnord3_svg_lineLinear,
  "step-before": fnord3_svg_lineStepBefore,
  "step-after": fnord3_svg_lineStepAfter,
  "basis": fnord3_svg_lineBasis,
  "basis-open": fnord3_svg_lineBasisOpen,
  "basis-closed": fnord3_svg_lineBasisClosed,
  "bundle": fnord3_svg_lineBundle,
  "cardinal": fnord3_svg_lineCardinal,
  "cardinal-open": fnord3_svg_lineCardinalOpen,
  "cardinal-closed": fnord3_svg_lineCardinalClosed,
  "monotone": fnord3_svg_lineMonotone
});

// Linear interpolation; generates "L" commands.
function fnord3_svg_lineLinear(points) {
  var i = 0,
      n = points.length,
      p = points[0],
      path = [p[0], ",", p[1]];
  while (++i < n) path.push("L", (p = points[i])[0], ",", p[1]);
  return path.join("");
}

// Step interpolation; generates "H" and "V" commands.
function fnord3_svg_lineStepBefore(points) {
  var i = 0,
      n = points.length,
      p = points[0],
      path = [p[0], ",", p[1]];
  while (++i < n) path.push("V", (p = points[i])[1], "H", p[0]);
  return path.join("");
}

// Step interpolation; generates "H" and "V" commands.
function fnord3_svg_lineStepAfter(points) {
  var i = 0,
      n = points.length,
      p = points[0],
      path = [p[0], ",", p[1]];
  while (++i < n) path.push("H", (p = points[i])[0], "V", p[1]);
  return path.join("");
}

// Open cardinal spline interpolation; generates "C" commands.
function fnord3_svg_lineCardinalOpen(points, tension) {
  return points.length < 4
      ? fnord3_svg_lineLinear(points)
      : points[1] + fnord3_svg_lineHermite(points.slice(1, points.length - 1),
        fnord3_svg_lineCardinalTangents(points, tension));
}

// Closed cardinal spline interpolation; generates "C" commands.
function fnord3_svg_lineCardinalClosed(points, tension) {
  return points.length < 3
      ? fnord3_svg_lineLinear(points)
      : points[0] + fnord3_svg_lineHermite((points.push(points[0]), points),
        fnord3_svg_lineCardinalTangents([points[points.length - 2]]
        .concat(points, [points[1]]), tension));
}

// Cardinal spline interpolation; generates "C" commands.
function fnord3_svg_lineCardinal(points, tension, closed) {
  return points.length < 3
      ? fnord3_svg_lineLinear(points)
      : points[0] + fnord3_svg_lineHermite(points,
        fnord3_svg_lineCardinalTangents(points, tension));
}

// Hermite spline construction; generates "C" commands.
function fnord3_svg_lineHermite(points, tangents) {
  if (tangents.length < 1
      || (points.length != tangents.length
      && points.length != tangents.length + 2)) {
    return fnord3_svg_lineLinear(points);
  }

  var quad = points.length != tangents.length,
      path = "",
      p0 = points[0],
      p = points[1],
      t0 = tangents[0],
      t = t0,
      pi = 1;

  if (quad) {
    path += "Q" + (p[0] - t0[0] * 2 / 3) + "," + (p[1] - t0[1] * 2 / 3)
        + "," + p[0] + "," + p[1];
    p0 = points[1];
    pi = 2;
  }

  if (tangents.length > 1) {
    t = tangents[1];
    p = points[pi];
    pi++;
    path += "C" + (p0[0] + t0[0]) + "," + (p0[1] + t0[1])
        + "," + (p[0] - t[0]) + "," + (p[1] - t[1])
        + "," + p[0] + "," + p[1];
    for (var i = 2; i < tangents.length; i++, pi++) {
      p = points[pi];
      t = tangents[i];
      path += "S" + (p[0] - t[0]) + "," + (p[1] - t[1])
          + "," + p[0] + "," + p[1];
    }
  }

  if (quad) {
    var lp = points[pi];
    path += "Q" + (p[0] + t[0] * 2 / 3) + "," + (p[1] + t[1] * 2 / 3)
        + "," + lp[0] + "," + lp[1];
  }

  return path;
}

// Generates tangents for a cardinal spline.
function fnord3_svg_lineCardinalTangents(points, tension) {
  var tangents = [],
      a = (1 - tension) / 2,
      p0,
      p1 = points[0],
      p2 = points[1],
      i = 1,
      n = points.length;
  while (++i < n) {
    p0 = p1;
    p1 = p2;
    p2 = points[i];
    tangents.push([a * (p2[0] - p0[0]), a * (p2[1] - p0[1])]);
  }
  return tangents;
}

// B-spline interpolation; generates "C" commands.
function fnord3_svg_lineBasis(points) {
  if (points.length < 3) return fnord3_svg_lineLinear(points);
  var i = 1,
      n = points.length,
      pi = points[0],
      x0 = pi[0],
      y0 = pi[1],
      px = [x0, x0, x0, (pi = points[1])[0]],
      py = [y0, y0, y0, pi[1]],
      path = [x0, ",", y0];
  fnord3_svg_lineBasisBezier(path, px, py);
  while (++i < n) {
    pi = points[i];
    px.shift(); px.push(pi[0]);
    py.shift(); py.push(pi[1]);
    fnord3_svg_lineBasisBezier(path, px, py);
  }
  i = -1;
  while (++i < 2) {
    px.shift(); px.push(pi[0]);
    py.shift(); py.push(pi[1]);
    fnord3_svg_lineBasisBezier(path, px, py);
  }
  return path.join("");
}

// Open B-spline interpolation; generates "C" commands.
function fnord3_svg_lineBasisOpen(points) {
  if (points.length < 4) return fnord3_svg_lineLinear(points);
  var path = [],
      i = -1,
      n = points.length,
      pi,
      px = [0],
      py = [0];
  while (++i < 3) {
    pi = points[i];
    px.push(pi[0]);
    py.push(pi[1]);
  }
  path.push(fnord3_svg_lineDot4(fnord3_svg_lineBasisBezier3, px)
    + "," + fnord3_svg_lineDot4(fnord3_svg_lineBasisBezier3, py));
  --i; while (++i < n) {
    pi = points[i];
    px.shift(); px.push(pi[0]);
    py.shift(); py.push(pi[1]);
    fnord3_svg_lineBasisBezier(path, px, py);
  }
  return path.join("");
}

// Closed B-spline interpolation; generates "C" commands.
function fnord3_svg_lineBasisClosed(points) {
  var path,
      i = -1,
      n = points.length,
      m = n + 4,
      pi,
      px = [],
      py = [];
  while (++i < 4) {
    pi = points[i % n];
    px.push(pi[0]);
    py.push(pi[1]);
  }
  path = [
    fnord3_svg_lineDot4(fnord3_svg_lineBasisBezier3, px), ",",
    fnord3_svg_lineDot4(fnord3_svg_lineBasisBezier3, py)
  ];
  --i; while (++i < m) {
    pi = points[i % n];
    px.shift(); px.push(pi[0]);
    py.shift(); py.push(pi[1]);
    fnord3_svg_lineBasisBezier(path, px, py);
  }
  return path.join("");
}

function fnord3_svg_lineBundle(points, tension) {
  var n = points.length - 1,
      x0 = points[0][0],
      y0 = points[0][1],
      dx = points[n][0] - x0,
      dy = points[n][1] - y0,
      i = -1,
      p,
      t;
  while (++i <= n) {
    p = points[i];
    t = i / n;
    p[0] = tension * p[0] + (1 - tension) * (x0 + t * dx);
    p[1] = tension * p[1] + (1 - tension) * (y0 + t * dy);
  }
  return fnord3_svg_lineBasis(points);
}

// Returns the dot product of the given four-element vectors.
function fnord3_svg_lineDot4(a, b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

// Matrix to transform basis (b-spline) control points to bezier
// control points. Derived from FvD 11.2.8.
var fnord3_svg_lineBasisBezier1 = [0, 2/3, 1/3, 0],
    fnord3_svg_lineBasisBezier2 = [0, 1/3, 2/3, 0],
    fnord3_svg_lineBasisBezier3 = [0, 1/6, 2/3, 1/6];

// Pushes a "C" Bézier curve onto the specified path array, given the
// two specified four-element arrays which define the control points.
function fnord3_svg_lineBasisBezier(path, x, y) {
  path.push(
      "C", fnord3_svg_lineDot4(fnord3_svg_lineBasisBezier1, x),
      ",", fnord3_svg_lineDot4(fnord3_svg_lineBasisBezier1, y),
      ",", fnord3_svg_lineDot4(fnord3_svg_lineBasisBezier2, x),
      ",", fnord3_svg_lineDot4(fnord3_svg_lineBasisBezier2, y),
      ",", fnord3_svg_lineDot4(fnord3_svg_lineBasisBezier3, x),
      ",", fnord3_svg_lineDot4(fnord3_svg_lineBasisBezier3, y));
}

// Computes the slope from points p0 to p1.
function fnord3_svg_lineSlope(p0, p1) {
  return (p1[1] - p0[1]) / (p1[0] - p0[0]);
}

// Compute three-point differences for the given points.
// http://en.wikipedia.org/wiki/Cubic_Hermite_spline#Finite_difference
function fnord3_svg_lineFiniteDifferences(points) {
  var i = 0,
      j = points.length - 1,
      m = [],
      p0 = points[0],
      p1 = points[1],
      d = m[0] = fnord3_svg_lineSlope(p0, p1);
  while (++i < j) {
    m[i] = d + (d = fnord3_svg_lineSlope(p0 = p1, p1 = points[i + 1]));
  }
  m[i] = d;
  return m;
}

// Interpolates the given points using Fritsch-Carlson Monotone cubic Hermite
// interpolation. Returns an array of tangent vectors. For details, see
// http://en.wikipedia.org/wiki/Monotone_cubic_interpolation
function fnord3_svg_lineMonotoneTangents(points) {
  var tangents = [],
      d,
      a,
      b,
      s,
      m = fnord3_svg_lineFiniteDifferences(points),
      i = -1,
      j = points.length - 1;

  // The first two steps are done by computing finite-differences:
  // 1. Compute the slopes of the secant lines between successive points.
  // 2. Initialize the tangents at every point as the average of the secants.

  // Then, for each segment…
  while (++i < j) {
    d = fnord3_svg_lineSlope(points[i], points[i + 1]);

    // 3. If two successive yk = y{k + 1} are equal (i.e., d is zero), then set
    // mk = m{k + 1} = 0 as the spline connecting these points must be flat to
    // preserve monotonicity. Ignore step 4 and 5 for those k.

    if (Math.abs(d) < 1e-6) {
      m[i] = m[i + 1] = 0;
    } else {
      // 4. Let ak = mk / dk and bk = m{k + 1} / dk.
      a = m[i] / d;
      b = m[i + 1] / d;

      // 5. Prevent overshoot and ensure monotonicity by restricting the
      // magnitude of vector <ak, bk> to a circle of radius 3.
      s = a * a + b * b;
      if (s > 9) {
        s = d * 3 / Math.sqrt(s);
        m[i] = s * a;
        m[i + 1] = s * b;
      }
    }
  }

  // Compute the normalized tangent vector from the slopes. Note that if x is
  // not monotonic, it's possible that the slope will be infinite, so we protect
  // against NaN by setting the coordinate to zero.
  i = -1; while (++i <= j) {
    s = (points[Math.min(j, i + 1)][0] - points[Math.max(0, i - 1)][0]) / (6 * (1 + m[i] * m[i]));
    tangents.push([s || 0, m[i] * s || 0]);
  }

  return tangents;
}

function fnord3_svg_lineMonotone(points) {
  return points.length < 3
      ? fnord3_svg_lineLinear(points)
      : points[0] + fnord3_svg_lineHermite(points, fnord3_svg_lineMonotoneTangents(points));
}
fnord3.svg.line.radial = function() {
  var line = fnord3_svg_line(fnord3_svg_lineRadial);
  line.radius = line.x, delete line.x;
  line.angle = line.y, delete line.y;
  return line;
};

function fnord3_svg_lineRadial(points) {
  var point,
      i = -1,
      n = points.length,
      r,
      a;
  while (++i < n) {
    point = points[i];
    r = point[0];
    a = point[1] + fnord3_svg_arcOffset;
    point[0] = r * Math.cos(a);
    point[1] = r * Math.sin(a);
  }
  return points;
}
function fnord3_svg_area(projection) {
  var x0 = fnord3_svg_lineX,
      x1 = fnord3_svg_lineX,
      y0 = 0,
      y1 = fnord3_svg_lineY,
      defined = fnord3_true,
      interpolate = fnord3_svg_lineInterpolatorDefault,
      i0 = fnord3_svg_lineLinear,
      i1 = fnord3_svg_lineLinear,
      L = "L",
      tension = .7;

  function area(data) {
    var segments = [],
        points0 = [],
        points1 = [],
        i = -1,
        n = data.length,
        d,
        fx0 = fnord3_functor(x0),
        fy0 = fnord3_functor(y0),
        fx1 = x0 === x1 ? function() { return x; } : fnord3_functor(x1),
        fy1 = y0 === y1 ? function() { return y; } : fnord3_functor(y1),
        x,
        y;

    function segment() {
      segments.push("M", i0(projection(points1), tension),
          L, i1(projection(points0.reverse()), tension),
          "Z");
    }

    while (++i < n) {
      if (defined.call(this, d = data[i], i)) {
        points0.push([x = +fx0.call(this, d, i), y = +fy0.call(this, d, i)]);
        points1.push([+fx1.call(this, d, i), +fy1.call(this, d, i)]);
      } else if (points0.length) {
        segment();
        points0 = [];
        points1 = [];
      }
    }

    if (points0.length) segment();

    return segments.length ? segments.join("") : null;
  }

  area.x = function(_) {
    if (!arguments.length) return x1;
    x0 = x1 = _;
    return area;
  };

  area.x0 = function(_) {
    if (!arguments.length) return x0;
    x0 = _;
    return area;
  };

  area.x1 = function(_) {
    if (!arguments.length) return x1;
    x1 = _;
    return area;
  };

  area.y = function(_) {
    if (!arguments.length) return y1;
    y0 = y1 = _;
    return area;
  };

  area.y0 = function(_) {
    if (!arguments.length) return y0;
    y0 = _;
    return area;
  };

  area.y1 = function(_) {
    if (!arguments.length) return y1;
    y1 = _;
    return area;
  };

  area.defined  = function(_) {
    if (!arguments.length) return defined;
    defined = _;
    return area;
  };

  area.interpolate = function(_) {
    if (!arguments.length) return interpolate;
    if (!fnord3_svg_lineInterpolators.has(_ += "")) _ = fnord3_svg_lineInterpolatorDefault;
    i0 = fnord3_svg_lineInterpolators.get(interpolate = _);
    i1 = i0.reverse || i0;
    L = /-closed$/.test(_) ? "M" : "L";
    return area;
  };

  area.tension = function(_) {
    if (!arguments.length) return tension;
    tension = _;
    return area;
  };

  return area;
}

fnord3_svg_lineStepBefore.reverse = fnord3_svg_lineStepAfter;
fnord3_svg_lineStepAfter.reverse = fnord3_svg_lineStepBefore;

fnord3.svg.area = function() {
  return fnord3_svg_area(Object);
};
fnord3.svg.area.radial = function() {
  var area = fnord3_svg_area(fnord3_svg_lineRadial);
  area.radius = area.x, delete area.x;
  area.innerRadius = area.x0, delete area.x0;
  area.outerRadius = area.x1, delete area.x1;
  area.angle = area.y, delete area.y;
  area.startAngle = area.y0, delete area.y0;
  area.endAngle = area.y1, delete area.y1;
  return area;
};
fnord3.svg.chord = function() {
  var source = fnord3_svg_chordSource,
      target = fnord3_svg_chordTarget,
      radius = fnord3_svg_chordRadius,
      startAngle = fnord3_svg_arcStartAngle,
      endAngle = fnord3_svg_arcEndAngle;

  // TODO Allow control point to be customized.

  function chord(d, i) {
    var s = subgroup(this, source, d, i),
        t = subgroup(this, target, d, i);
    return "M" + s.p0
      + arc(s.r, s.p1, s.a1 - s.a0) + (equals(s, t)
      ? curve(s.r, s.p1, s.r, s.p0)
      : curve(s.r, s.p1, t.r, t.p0)
      + arc(t.r, t.p1, t.a1 - t.a0)
      + curve(t.r, t.p1, s.r, s.p0))
      + "Z";
  }

  function subgroup(self, f, d, i) {
    var subgroup = f.call(self, d, i),
        r = radius.call(self, subgroup, i),
        a0 = startAngle.call(self, subgroup, i) + fnord3_svg_arcOffset,
        a1 = endAngle.call(self, subgroup, i) + fnord3_svg_arcOffset;
    return {
      r: r,
      a0: a0,
      a1: a1,
      p0: [r * Math.cos(a0), r * Math.sin(a0)],
      p1: [r * Math.cos(a1), r * Math.sin(a1)]
    };
  }

  function equals(a, b) {
    return a.a0 == b.a0 && a.a1 == b.a1;
  }

  function arc(r, p, a) {
    return "A" + r + "," + r + " 0 " + +(a > Math.PI) + ",1 " + p;
  }

  function curve(r0, p0, r1, p1) {
    return "Q 0,0 " + p1;
  }

  chord.radius = function(v) {
    if (!arguments.length) return radius;
    radius = fnord3_functor(v);
    return chord;
  };

  chord.source = function(v) {
    if (!arguments.length) return source;
    source = fnord3_functor(v);
    return chord;
  };

  chord.target = function(v) {
    if (!arguments.length) return target;
    target = fnord3_functor(v);
    return chord;
  };

  chord.startAngle = function(v) {
    if (!arguments.length) return startAngle;
    startAngle = fnord3_functor(v);
    return chord;
  };

  chord.endAngle = function(v) {
    if (!arguments.length) return endAngle;
    endAngle = fnord3_functor(v);
    return chord;
  };

  return chord;
};

function fnord3_svg_chordSource(d) {
  return d.source;
}

function fnord3_svg_chordTarget(d) {
  return d.target;
}

function fnord3_svg_chordRadius(d) {
  return d.radius;
}

function fnord3_svg_chordStartAngle(d) {
  return d.startAngle;
}

function fnord3_svg_chordEndAngle(d) {
  return d.endAngle;
}
fnord3.svg.diagonal = function() {
  var source = fnord3_svg_chordSource,
      target = fnord3_svg_chordTarget,
      projection = fnord3_svg_diagonalProjection;

  function diagonal(d, i) {
    var p0 = source.call(this, d, i),
        p3 = target.call(this, d, i),
        m = (p0.y + p3.y) / 2,
        p = [p0, {x: p0.x, y: m}, {x: p3.x, y: m}, p3];
    p = p.map(projection);
    return "M" + p[0] + "C" + p[1] + " " + p[2] + " " + p[3];
  }

  diagonal.source = function(x) {
    if (!arguments.length) return source;
    source = fnord3_functor(x);
    return diagonal;
  };

  diagonal.target = function(x) {
    if (!arguments.length) return target;
    target = fnord3_functor(x);
    return diagonal;
  };

  diagonal.projection = function(x) {
    if (!arguments.length) return projection;
    projection = x;
    return diagonal;
  };

  return diagonal;
};

function fnord3_svg_diagonalProjection(d) {
  return [d.x, d.y];
}
fnord3.svg.diagonal.radial = function() {
  var diagonal = fnord3.svg.diagonal(),
      projection = fnord3_svg_diagonalProjection,
      projection_ = diagonal.projection;

  diagonal.projection = function(x) {
    return arguments.length
        ? projection_(fnord3_svg_diagonalRadialProjection(projection = x))
        : projection;
  };

  return diagonal;
};

function fnord3_svg_diagonalRadialProjection(projection) {
  return function() {
    var d = projection.apply(this, arguments),
        r = d[0],
        a = d[1] + fnord3_svg_arcOffset;
    return [r * Math.cos(a), r * Math.sin(a)];
  };
}
fnord3.svg.mouse = fnord3.mouse;
fnord3.svg.touches = fnord3.touches;
fnord3.svg.symbol = function() {
  var type = fnord3_svg_symbolType,
      size = fnord3_svg_symbolSize;

  function symbol(d, i) {
    return (fnord3_svg_symbols.get(type.call(this, d, i))
        || fnord3_svg_symbolCircle)
        (size.call(this, d, i));
  }

  symbol.type = function(x) {
    if (!arguments.length) return type;
    type = fnord3_functor(x);
    return symbol;
  };

  // size of symbol in square pixels
  symbol.size = function(x) {
    if (!arguments.length) return size;
    size = fnord3_functor(x);
    return symbol;
  };

  return symbol;
};

function fnord3_svg_symbolSize() {
  return 64;
}

function fnord3_svg_symbolType() {
  return "circle";
}

function fnord3_svg_symbolCircle(size) {
  var r = Math.sqrt(size / Math.PI);
  return "M0," + r
      + "A" + r + "," + r + " 0 1,1 0," + (-r)
      + "A" + r + "," + r + " 0 1,1 0," + r
      + "Z";
}

// TODO cross-diagonal?
var fnord3_svg_symbols = fnord3.map({
  "circle": fnord3_svg_symbolCircle,
  "cross": function(size) {
    var r = Math.sqrt(size / 5) / 2;
    return "M" + -3 * r + "," + -r
        + "H" + -r
        + "V" + -3 * r
        + "H" + r
        + "V" + -r
        + "H" + 3 * r
        + "V" + r
        + "H" + r
        + "V" + 3 * r
        + "H" + -r
        + "V" + r
        + "H" + -3 * r
        + "Z";
  },
  "diamond": function(size) {
    var ry = Math.sqrt(size / (2 * fnord3_svg_symbolTan30)),
        rx = ry * fnord3_svg_symbolTan30;
    return "M0," + -ry
        + "L" + rx + ",0"
        + " 0," + ry
        + " " + -rx + ",0"
        + "Z";
  },
  "square": function(size) {
    var r = Math.sqrt(size) / 2;
    return "M" + -r + "," + -r
        + "L" + r + "," + -r
        + " " + r + "," + r
        + " " + -r + "," + r
        + "Z";
  },
  "triangle-down": function(size) {
    var rx = Math.sqrt(size / fnord3_svg_symbolSqrt3),
        ry = rx * fnord3_svg_symbolSqrt3 / 2;
    return "M0," + ry
        + "L" + rx +"," + -ry
        + " " + -rx + "," + -ry
        + "Z";
  },
  "triangle-up": function(size) {
    var rx = Math.sqrt(size / fnord3_svg_symbolSqrt3),
        ry = rx * fnord3_svg_symbolSqrt3 / 2;
    return "M0," + -ry
        + "L" + rx +"," + ry
        + " " + -rx + "," + ry
        + "Z";
  }
});

fnord3.svg.symbolTypes = fnord3_svg_symbols.keys();

var fnord3_svg_symbolSqrt3 = Math.sqrt(3),
    fnord3_svg_symbolTan30 = Math.tan(30 * Math.PI / 180);
fnord3.svg.axis = function() {
  var scale = fnord3.scale.linear(),
      orient = "bottom",
      tickMajorSize = 6,
      tickMinorSize = 6,
      tickEndSize = 6,
      tickPadding = 3,
      tickArguments_ = [10],
      tickValues = null,
      tickFormat_,
      tickSubdivide = 0;

  function axis(g) {
    g.each(function() {
      var g = fnord3.select(this);

      // Ticks, or domain values for ordinal scales.
      var ticks = tickValues == null ? (scale.ticks ? scale.ticks.apply(scale, tickArguments_) : scale.domain()) : tickValues,
          tickFormat = tickFormat_ == null ? (scale.tickFormat ? scale.tickFormat.apply(scale, tickArguments_) : String) : tickFormat_;

      // Minor ticks.
      var subticks = fnord3_svg_axisSubdivide(scale, ticks, tickSubdivide),
          subtick = g.selectAll(".minor").data(subticks, String),
          subtickEnter = subtick.enter().insert("line", "g").attr("class", "tick minor").style("opacity", 1e-6),
          subtickExit = fnord3.transition(subtick.exit()).style("opacity", 1e-6).remove(),
          subtickUpdate = fnord3.transition(subtick).style("opacity", 1);

      // Major ticks.
      var tick = g.selectAll("g").data(ticks, String),
          tickEnter = tick.enter().insert("g", "path").style("opacity", 1e-6),
          tickExit = fnord3.transition(tick.exit()).style("opacity", 1e-6).remove(),
          tickUpdate = fnord3.transition(tick).style("opacity", 1),
          tickTransform;

      // Domain.
      var range = fnord3_scaleRange(scale),
          path = g.selectAll(".domain").data([0]),
          pathEnter = path.enter().append("path").attr("class", "domain"),
          pathUpdate = fnord3.transition(path);

      // Stash a snapshot of the new scale, and retrieve the old snapshot.
      var scale1 = scale.copy(),
          scale0 = this.__chart__ || scale1;
      this.__chart__ = scale1;

      tickEnter.append("line").attr("class", "tick");
      tickEnter.append("text");
      tickUpdate.select("text").text(tickFormat);

      switch (orient) {
        case "bottom": {
          tickTransform = fnord3_svg_axisX;
          subtickEnter.attr("y2", tickMinorSize);
          subtickUpdate.attr("x2", 0).attr("y2", tickMinorSize);
          tickEnter.select("line").attr("y2", tickMajorSize);
          tickEnter.select("text").attr("y", Math.max(tickMajorSize, 0) + tickPadding);
          tickUpdate.select("line").attr("x2", 0).attr("y2", tickMajorSize);
          tickUpdate.select("text").attr("x", 0).attr("y", Math.max(tickMajorSize, 0) + tickPadding).attr("dy", ".71em").attr("text-anchor", "middle");
          pathUpdate.attr("d", "M" + range[0] + "," + tickEndSize + "V0H" + range[1] + "V" + tickEndSize);
          break;
        }
        case "top": {
          tickTransform = fnord3_svg_axisX;
          subtickEnter.attr("y2", -tickMinorSize);
          subtickUpdate.attr("x2", 0).attr("y2", -tickMinorSize);
          tickEnter.select("line").attr("y2", -tickMajorSize);
          tickEnter.select("text").attr("y", -(Math.max(tickMajorSize, 0) + tickPadding));
          tickUpdate.select("line").attr("x2", 0).attr("y2", -tickMajorSize);
          tickUpdate.select("text").attr("x", 0).attr("y", -(Math.max(tickMajorSize, 0) + tickPadding)).attr("dy", "0em").attr("text-anchor", "middle");
          pathUpdate.attr("d", "M" + range[0] + "," + -tickEndSize + "V0H" + range[1] + "V" + -tickEndSize);
          break;
        }
        case "left": {
          tickTransform = fnord3_svg_axisY;
          subtickEnter.attr("x2", -tickMinorSize);
          subtickUpdate.attr("x2", -tickMinorSize).attr("y2", 0);
          tickEnter.select("line").attr("x2", -tickMajorSize);
          tickEnter.select("text").attr("x", -(Math.max(tickMajorSize, 0) + tickPadding));
          tickUpdate.select("line").attr("x2", -tickMajorSize).attr("y2", 0);
          tickUpdate.select("text").attr("x", -(Math.max(tickMajorSize, 0) + tickPadding)).attr("y", 0).attr("dy", ".32em").attr("text-anchor", "end");
          pathUpdate.attr("d", "M" + -tickEndSize + "," + range[0] + "H0V" + range[1] + "H" + -tickEndSize);
          break;
        }
        case "right": {
          tickTransform = fnord3_svg_axisY;
          subtickEnter.attr("x2", tickMinorSize);
          subtickUpdate.attr("x2", tickMinorSize).attr("y2", 0);
          tickEnter.select("line").attr("x2", tickMajorSize);
          tickEnter.select("text").attr("x", Math.max(tickMajorSize, 0) + tickPadding);
          tickUpdate.select("line").attr("x2", tickMajorSize).attr("y2", 0);
          tickUpdate.select("text").attr("x", Math.max(tickMajorSize, 0) + tickPadding).attr("y", 0).attr("dy", ".32em").attr("text-anchor", "start");
          pathUpdate.attr("d", "M" + tickEndSize + "," + range[0] + "H0V" + range[1] + "H" + tickEndSize);
          break;
        }
      }

      // For quantitative scales:
      // - enter new ticks from the old scale
      // - exit old ticks to the new scale
      if (scale.ticks) {
        tickEnter.call(tickTransform, scale0);
        tickUpdate.call(tickTransform, scale1);
        tickExit.call(tickTransform, scale1);
        subtickEnter.call(tickTransform, scale0);
        subtickUpdate.call(tickTransform, scale1);
        subtickExit.call(tickTransform, scale1);
      }

      // For ordinal scales:
      // - any entering ticks are undefined in the old scale
      // - any exiting ticks are undefined in the new scale
      // Therefore, we only need to transition updating ticks.
      else {
        var dx = scale1.rangeBand() / 2, x = function(d) { return scale1(d) + dx; };
        tickEnter.call(tickTransform, x);
        tickUpdate.call(tickTransform, x);
      }
    });
  }

  axis.scale = function(x) {
    if (!arguments.length) return scale;
    scale = x;
    return axis;
  };

  axis.orient = function(x) {
    if (!arguments.length) return orient;
    orient = x;
    return axis;
  };

  axis.ticks = function() {
    if (!arguments.length) return tickArguments_;
    tickArguments_ = arguments;
    return axis;
  };

  axis.tickValues = function(x) {
    if (!arguments.length) return tickValues;
    tickValues = x;
    return axis;
  };

  axis.tickFormat = function(x) {
    if (!arguments.length) return tickFormat_;
    tickFormat_ = x;
    return axis;
  };

  axis.tickSize = function(x, y, z) {
    if (!arguments.length) return tickMajorSize;
    var n = arguments.length - 1;
    tickMajorSize = +x;
    tickMinorSize = n > 1 ? +y : tickMajorSize;
    tickEndSize = n > 0 ? +arguments[n] : tickMajorSize;
    return axis;
  };

  axis.tickPadding = function(x) {
    if (!arguments.length) return tickPadding;
    tickPadding = +x;
    return axis;
  };

  axis.tickSubdivide = function(x) {
    if (!arguments.length) return tickSubdivide;
    tickSubdivide = +x;
    return axis;
  };

  return axis;
};

function fnord3_svg_axisX(selection, x) {
  selection.attr("transform", function(d) { return "translate(" + x(d) + ",0)"; });
}

function fnord3_svg_axisY(selection, y) {
  selection.attr("transform", function(d) { return "translate(0," + y(d) + ")"; });
}

function fnord3_svg_axisSubdivide(scale, ticks, m) {
  subticks = [];
  if (m && ticks.length > 1) {
    var extent = fnord3_scaleExtent(scale.domain()),
        subticks,
        i = -1,
        n = ticks.length,
        d = (ticks[1] - ticks[0]) / ++m,
        j,
        v;
    while (++i < n) {
      for (j = m; --j > 0;) {
        if ((v = +ticks[i] - j * d) >= extent[0]) {
          subticks.push(v);
        }
      }
    }
    for (--i, j = 0; ++j < m && (v = +ticks[i] + j * d) < extent[1];) {
      subticks.push(v);
    }
  }
  return subticks;
}
fnord3.svg.brush = function() {
  var event = fnord3_eventDispatch(brush, "brushstart", "brush", "brushend"),
      x = null, // x-scale, optional
      y = null, // y-scale, optional
      resizes = fnord3_svg_brushResizes[0],
      extent = [[0, 0], [0, 0]], // [x0, y0], [x1, y1], in pixels (integers)
      extentDomain; // the extent in data space, lazily created

  function brush(g) {
    g.each(function() {
      var g = fnord3.select(this),
          bg = g.selectAll(".background").data([0]),
          fg = g.selectAll(".extent").data([0]),
          tz = g.selectAll(".resize").data(resizes, String),
          e;

      // Prepare the brush container for events.
      g
          .style("pointer-events", "all")
          .on("mousedown.brush", brushstart)
          .on("touchstart.brush", brushstart);

      // An invisible, mouseable area for starting a new brush.
      bg.enter().append("rect")
          .attr("class", "background")
          .style("visibility", "hidden")
          .style("cursor", "crosshair");

      // The visible brush extent; style this as you like!
      fg.enter().append("rect")
          .attr("class", "extent")
          .style("cursor", "move");

      // More invisible rects for resizing the extent.
      tz.enter().append("g")
          .attr("class", function(d) { return "resize " + d; })
          .style("cursor", function(d) { return fnord3_svg_brushCursor[d]; })
        .append("rect")
          .attr("x", function(d) { return /[ew]$/.test(d) ? -3 : null; })
          .attr("y", function(d) { return /^[ns]/.test(d) ? -3 : null; })
          .attr("width", 6)
          .attr("height", 6)
          .style("visibility", "hidden");

      // Show or hide the resizers.
      tz.style("display", brush.empty() ? "none" : null);

      // Remove any superfluous resizers.
      tz.exit().remove();

      // Initialize the background to fill the defined range.
      // If the range isn't defined, you can post-process.
      if (x) {
        e = fnord3_scaleRange(x);
        bg.attr("x", e[0]).attr("width", e[1] - e[0]);
        redrawX(g);
      }
      if (y) {
        e = fnord3_scaleRange(y);
        bg.attr("y", e[0]).attr("height", e[1] - e[0]);
        redrawY(g);
      }
      redraw(g);
    });
  }

  function redraw(g) {
    g.selectAll(".resize").attr("transform", function(d) {
      return "translate(" + extent[+/e$/.test(d)][0] + "," + extent[+/^s/.test(d)][1] + ")";
    });
  }

  function redrawX(g) {
    g.select(".extent").attr("x", extent[0][0]);
    g.selectAll(".extent,.n>rect,.s>rect").attr("width", extent[1][0] - extent[0][0]);
  }

  function redrawY(g) {
    g.select(".extent").attr("y", extent[0][1]);
    g.selectAll(".extent,.e>rect,.w>rect").attr("height", extent[1][1] - extent[0][1]);
  }

  function brushstart() {
    var target = this,
        eventTarget = fnord3.select(fnord3.event.target),
        event_ = event.of(target, arguments),
        g = fnord3.select(target),
        resizing = eventTarget.datum(),
        resizingX = !/^(n|s)$/.test(resizing) && x,
        resizingY = !/^(e|w)$/.test(resizing) && y,
        dragging = eventTarget.classed("extent"),
        center,
        origin = mouse(),
        offset;

    var w = fnord3.select(window)
        .on("mousemove.brush", brushmove)
        .on("mouseup.brush", brushend)
        .on("touchmove.brush", brushmove)
        .on("touchend.brush", brushend)
        .on("keydown.brush", keydown)
        .on("keyup.brush", keyup);

    // If the extent was clicked on, drag rather than brush;
    // store the point between the mouse and extent origin instead.
    if (dragging) {
      origin[0] = extent[0][0] - origin[0];
      origin[1] = extent[0][1] - origin[1];
    }

    // If a resizer was clicked on, record which side is to be resized.
    // Also, set the origin to the opposite side.
    else if (resizing) {
      var ex = +/w$/.test(resizing),
          ey = +/^n/.test(resizing);
      offset = [extent[1 - ex][0] - origin[0], extent[1 - ey][1] - origin[1]];
      origin[0] = extent[ex][0];
      origin[1] = extent[ey][1];
    }

    // If the ALT key is down when starting a brush, the center is at the mouse.
    else if (fnord3.event.altKey) center = origin.slice();

    // Propagate the active cursor to the body for the drag duration.
    g.style("pointer-events", "none").selectAll(".resize").style("display", null);
    fnord3.select("body").style("cursor", eventTarget.style("cursor"));

    // Notify listeners.
    event_({type: "brushstart"});
    brushmove();
    fnord3_eventCancel();

    function mouse() {
      var touches = fnord3.event.changedTouches;
      return touches ? fnord3.touches(target, touches)[0] : fnord3.mouse(target);
    }

    function keydown() {
      if (fnord3.event.keyCode == 32) {
        if (!dragging) {
          center = null;
          origin[0] -= extent[1][0];
          origin[1] -= extent[1][1];
          dragging = 2;
        }
        fnord3_eventCancel();
      }
    }

    function keyup() {
      if (fnord3.event.keyCode == 32 && dragging == 2) {
        origin[0] += extent[1][0];
        origin[1] += extent[1][1];
        dragging = 0;
        fnord3_eventCancel();
      }
    }

    function brushmove() {
      var point = mouse(),
          moved = false;

      // Preserve the offset for thick resizers.
      if (offset) {
        point[0] += offset[0];
        point[1] += offset[1];
      }

      if (!dragging) {

        // If needed, determine the center from the current extent.
        if (fnord3.event.altKey) {
          if (!center) center = [(extent[0][0] + extent[1][0]) / 2, (extent[0][1] + extent[1][1]) / 2];

          // Update the origin, for when the ALT key is released.
          origin[0] = extent[+(point[0] < center[0])][0];
          origin[1] = extent[+(point[1] < center[1])][1];
        }

        // When the ALT key is released, we clear the center.
        else center = null;
      }

      // Update the brush extent for each dimension.
      if (resizingX && move1(point, x, 0)) {
        redrawX(g);
        moved = true;
      }
      if (resizingY && move1(point, y, 1)) {
        redrawY(g);
        moved = true;
      }

      // Final redraw and notify listeners.
      if (moved) {
        redraw(g);
        event_({type: "brush", mode: dragging ? "move" : "resize"});
      }
    }

    function move1(point, scale, i) {
      var range = fnord3_scaleRange(scale),
          r0 = range[0],
          r1 = range[1],
          position = origin[i],
          size = extent[1][i] - extent[0][i],
          min,
          max;

      // When dragging, reduce the range by the extent size and position.
      if (dragging) {
        r0 -= position;
        r1 -= size + position;
      }

      // Clamp the point so that the extent fits within the range extent.
      min = Math.max(r0, Math.min(r1, point[i]));

      // Compute the new extent bounds.
      if (dragging) {
        max = (min += position) + size;
      } else {

        // If the ALT key is pressed, then preserve the center of the extent.
        if (center) position = Math.max(r0, Math.min(r1, 2 * center[i] - min));

        // Compute the min and max of the position and point.
        if (position < min) {
          max = min;
          min = position;
        } else {
          max = position;
        }
      }

      // Update the stored bounds.
      if (extent[0][i] !== min || extent[1][i] !== max) {
        extentDomain = null;
        extent[0][i] = min;
        extent[1][i] = max;
        return true;
      }
    }

    function brushend() {
      brushmove();

      // reset the cursor styles
      g.style("pointer-events", "all").selectAll(".resize").style("display", brush.empty() ? "none" : null);
      fnord3.select("body").style("cursor", null);

      w .on("mousemove.brush", null)
        .on("mouseup.brush", null)
        .on("touchmove.brush", null)
        .on("touchend.brush", null)
        .on("keydown.brush", null)
        .on("keyup.brush", null);

      event_({type: "brushend"});
      fnord3_eventCancel();
    }
  }

  brush.x = function(z) {
    if (!arguments.length) return x;
    x = z;
    resizes = fnord3_svg_brushResizes[!x << 1 | !y]; // fore!
    return brush;
  };

  brush.y = function(z) {
    if (!arguments.length) return y;
    y = z;
    resizes = fnord3_svg_brushResizes[!x << 1 | !y]; // fore!
    return brush;
  };

  brush.extent = function(z) {
    var x0, x1, y0, y1, t;

    // Invert the pixel extent to data-space.
    if (!arguments.length) {
      z = extentDomain || extent;
      if (x) {
        x0 = z[0][0], x1 = z[1][0];
        if (!extentDomain) {
          x0 = extent[0][0], x1 = extent[1][0];
          if (x.invert) x0 = x.invert(x0), x1 = x.invert(x1);
          if (x1 < x0) t = x0, x0 = x1, x1 = t;
        }
      }
      if (y) {
        y0 = z[0][1], y1 = z[1][1];
        if (!extentDomain) {
          y0 = extent[0][1], y1 = extent[1][1];
          if (y.invert) y0 = y.invert(y0), y1 = y.invert(y1);
          if (y1 < y0) t = y0, y0 = y1, y1 = t;
        }
      }
      return x && y ? [[x0, y0], [x1, y1]] : x ? [x0, x1] : y && [y0, y1];
    }

    // Scale the data-space extent to pixels.
    extentDomain = [[0, 0], [0, 0]];
    if (x) {
      x0 = z[0], x1 = z[1];
      if (y) x0 = x0[0], x1 = x1[0];
      extentDomain[0][0] = x0, extentDomain[1][0] = x1;
      if (x.invert) x0 = x(x0), x1 = x(x1);
      if (x1 < x0) t = x0, x0 = x1, x1 = t;
      extent[0][0] = x0 | 0, extent[1][0] = x1 | 0;
    }
    if (y) {
      y0 = z[0], y1 = z[1];
      if (x) y0 = y0[1], y1 = y1[1];
      extentDomain[0][1] = y0, extentDomain[1][1] = y1;
      if (y.invert) y0 = y(y0), y1 = y(y1);
      if (y1 < y0) t = y0, y0 = y1, y1 = t;
      extent[0][1] = y0 | 0, extent[1][1] = y1 | 0;
    }

    return brush;
  };

  brush.clear = function() {
    extentDomain = null;
    extent[0][0] =
    extent[0][1] =
    extent[1][0] =
    extent[1][1] = 0;
    return brush;
  };

  brush.empty = function() {
    return (x && extent[0][0] === extent[1][0])
        || (y && extent[0][1] === extent[1][1]);
  };

  return fnord3.rebind(brush, event, "on");
};

var fnord3_svg_brushCursor = {
  n: "ns-resize",
  e: "ew-resize",
  s: "ns-resize",
  w: "ew-resize",
  nw: "nwse-resize",
  ne: "nesw-resize",
  se: "nwse-resize",
  sw: "nesw-resize"
};

var fnord3_svg_brushResizes = [
  ["n", "e", "s", "w", "nw", "ne", "se", "sw"],
  ["e", "w"],
  ["n", "s"],
  []
];
fnord3.behavior = {};
// TODO Track touch points by identifier.

fnord3.behavior.drag = function() {
  var event = fnord3_eventDispatch(drag, "drag", "dragstart", "dragend"),
      origin = null;

  function drag() {
    this.on("mousedown.drag", mousedown)
        .on("touchstart.drag", mousedown);
  }

  function mousedown() {
    var target = this,
        event_ = event.of(target, arguments),
        eventTarget = fnord3.event.target,
        offset,
        origin_ = point(),
        moved = 0;

    var w = fnord3.select(window)
        .on("mousemove.drag", dragmove)
        .on("touchmove.drag", dragmove)
        .on("mouseup.drag", dragend, true)
        .on("touchend.drag", dragend, true);

    if (origin) {
      offset = origin.apply(target, arguments);
      offset = [offset.x - origin_[0], offset.y - origin_[1]];
    } else {
      offset = [0, 0];
    }

    fnord3_eventCancel();
    event_({type: "dragstart"});

    function point() {
      var p = target.parentNode,
          t = fnord3.event.changedTouches;
      return t ? fnord3.touches(p, t)[0] : fnord3.mouse(p);
    }

    function dragmove() {
      if (!target.parentNode) return dragend(); // target removed from DOM

      var p = point(),
          dx = p[0] - origin_[0],
          dy = p[1] - origin_[1];

      moved |= dx | dy;
      origin_ = p;
      fnord3_eventCancel();

      event_({type: "drag", x: p[0] + offset[0], y: p[1] + offset[1], dx: dx, dy: dy});
    }

    function dragend() {
      event_({type: "dragend"});

      // if moved, prevent the mouseup (and possibly click) from propagating
      if (moved) {
        fnord3_eventCancel();
        if (fnord3.event.target === eventTarget) w.on("click.drag", click, true);
      }

      w .on("mousemove.drag", null)
        .on("touchmove.drag", null)
        .on("mouseup.drag", null)
        .on("touchend.drag", null);
    }

    // prevent the subsequent click from propagating (e.g., for anchors)
    function click() {
      fnord3_eventCancel();
      w.on("click.drag", null);
    }
  }

  drag.origin = function(x) {
    if (!arguments.length) return origin;
    origin = x;
    return drag;
  };

  return fnord3.rebind(drag, event, "on");
};
fnord3.behavior.zoom = function() {
  var translate = [0, 0],
      translate0, // translate when we started zooming (to avoid drift)
      scale = 1,
      scale0, // scale when we started touching
      scaleExtent = fnord3_behavior_zoomInfinity,
      event = fnord3_eventDispatch(zoom, "zoom"),
      x0,
      x1,
      y0,
      y1,
      touchtime; // time of last touchstart (to detect double-tap)

  function zoom() {
    this
        .on("mousedown.zoom", mousedown)
        .on("mousewheel.zoom", mousewheel)
        .on("mousemove.zoom", mousemove)
        .on("DOMMouseScroll.zoom", mousewheel)
        .on("dblclick.zoom", dblclick)
        .on("touchstart.zoom", touchstart)
        .on("touchmove.zoom", touchmove)
        .on("touchend.zoom", touchstart);
  }

  zoom.translate = function(x) {
    if (!arguments.length) return translate;
    translate = x.map(Number);
    return zoom;
  };

  zoom.scale = function(x) {
    if (!arguments.length) return scale;
    scale = +x;
    return zoom;
  };

  zoom.scaleExtent = function(x) {
    if (!arguments.length) return scaleExtent;
    scaleExtent = x == null ? fnord3_behavior_zoomInfinity : x.map(Number);
    return zoom;
  };

  zoom.x = function(z) {
    if (!arguments.length) return x1;
    x1 = z;
    x0 = z.copy();
    return zoom;
  };

  zoom.y = function(z) {
    if (!arguments.length) return y1;
    y1 = z;
    y0 = z.copy();
    return zoom;
  };

  function location(p) {
    return [(p[0] - translate[0]) / scale, (p[1] - translate[1]) / scale];
  }

  function point(l) {
    return [l[0] * scale + translate[0], l[1] * scale + translate[1]];
  }

  function scaleTo(s) {
    scale = Math.max(scaleExtent[0], Math.min(scaleExtent[1], s));
  }

  function translateTo(p, l) {
    l = point(l);
    translate[0] += p[0] - l[0];
    translate[1] += p[1] - l[1];
  }

  function dispatch(event) {
    if (x1) x1.domain(x0.range().map(function(x) { return (x - translate[0]) / scale; }).map(x0.invert));
    if (y1) y1.domain(y0.range().map(function(y) { return (y - translate[1]) / scale; }).map(y0.invert));
    fnord3.event.preventDefault();
    event({type: "zoom", scale: scale, translate: translate});
  }

  function mousedown() {
    var target = this,
        event_ = event.of(target, arguments),
        eventTarget = fnord3.event.target,
        moved = 0,
        w = fnord3.select(window).on("mousemove.zoom", mousemove).on("mouseup.zoom", mouseup),
        l = location(fnord3.mouse(target));

    window.focus();
    fnord3_eventCancel();

    function mousemove() {
      moved = 1;
      translateTo(fnord3.mouse(target), l);
      dispatch(event_);
    }

    function mouseup() {
      if (moved) fnord3_eventCancel();
      w.on("mousemove.zoom", null).on("mouseup.zoom", null);
      if (moved && fnord3.event.target === eventTarget) w.on("click.zoom", click, true);
    }

    function click() {
      fnord3_eventCancel();
      w.on("click.zoom", null);
    }
  }

  function mousewheel() {
    if (!translate0) translate0 = location(fnord3.mouse(this));
    scaleTo(Math.pow(2, fnord3_behavior_zoomDelta() * .002) * scale);
    translateTo(fnord3.mouse(this), translate0);
    dispatch(event.of(this, arguments));
  }

  function mousemove() {
    translate0 = null;
  }

  function dblclick() {
    var p = fnord3.mouse(this), l = location(p);
    scaleTo(fnord3.event.shiftKey ? scale / 2 : scale * 2);
    translateTo(p, l);
    dispatch(event.of(this, arguments));
  }

  function touchstart() {
    var touches = fnord3.touches(this),
        now = Date.now();

    scale0 = scale;
    translate0 = {};
    touches.forEach(function(t) { translate0[t.identifier] = location(t); });
    fnord3_eventCancel();

    if ((touches.length === 1) && (now - touchtime < 500)) { // dbltap
      var p = touches[0], l = location(touches[0]);
      scaleTo(scale * 2);
      translateTo(p, l);
      dispatch(event.of(this, arguments));
    }
    touchtime = now;
  }

  function touchmove() {
    var touches = fnord3.touches(this),
        p0 = touches[0],
        l0 = translate0[p0.identifier];
    if (p1 = touches[1]) {
      var p1, l1 = translate0[p1.identifier];
      p0 = [(p0[0] + p1[0]) / 2, (p0[1] + p1[1]) / 2];
      l0 = [(l0[0] + l1[0]) / 2, (l0[1] + l1[1]) / 2];
      scaleTo(fnord3.event.scale * scale0);
    }
    translateTo(p0, l0);
    dispatch(event.of(this, arguments));
  }

  return fnord3.rebind(zoom, event, "on");
};

var fnord3_behavior_zoomDiv, // for interpreting mousewheel events
    fnord3_behavior_zoomInfinity = [0, Infinity]; // default scale extent

function fnord3_behavior_zoomDelta() {

  // mousewheel events are totally broken!
  // https://bugs.webkit.org/show_bug.cgi?id=40441
  // not only that, but Chrome and Safari differ in re. to acceleration!
  if (!fnord3_behavior_zoomDiv) {
    fnord3_behavior_zoomDiv = fnord3.select("body").append("div")
        .style("visibility", "hidden")
        .style("top", 0)
        .style("height", 0)
        .style("width", 0)
        .style("overflow-y", "scroll")
      .append("div")
        .style("height", "2000px")
      .node().parentNode;
  }

  var e = fnord3.event, delta;
  try {
    fnord3_behavior_zoomDiv.scrollTop = 1000;
    fnord3_behavior_zoomDiv.dispatchEvent(e);
    delta = 1000 - fnord3_behavior_zoomDiv.scrollTop;
  } catch (error) {
    delta = e.wheelDelta || (-e.detail * 5);
  }

  return delta;
}
fnord3.layout = {};
// Implements hierarchical edge bundling using Holten's algorithm. For each
// input link, a path is computed that travels through the tree, up the parent
// hierarchy to the least common ancestor, and then back down to the destination
// node. Each path is simply an array of nodes.
fnord3.layout.bundle = function() {
  return function(links) {
    var paths = [],
        i = -1,
        n = links.length;
    while (++i < n) paths.push(fnord3_layout_bundlePath(links[i]));
    return paths;
  };
};

function fnord3_layout_bundlePath(link) {
  var start = link.source,
      end = link.target,
      lca = fnord3_layout_bundleLeastCommonAncestor(start, end),
      points = [start];
  while (start !== lca) {
    start = start.parent;
    points.push(start);
  }
  var k = points.length;
  while (end !== lca) {
    points.splice(k, 0, end);
    end = end.parent;
  }
  return points;
}

function fnord3_layout_bundleAncestors(node) {
  var ancestors = [],
      parent = node.parent;
  while (parent != null) {
    ancestors.push(node);
    node = parent;
    parent = parent.parent;
  }
  ancestors.push(node);
  return ancestors;
}

function fnord3_layout_bundleLeastCommonAncestor(a, b) {
  if (a === b) return a;
  var aNodes = fnord3_layout_bundleAncestors(a),
      bNodes = fnord3_layout_bundleAncestors(b),
      aNode = aNodes.pop(),
      bNode = bNodes.pop(),
      sharedNode = null;
  while (aNode === bNode) {
    sharedNode = aNode;
    aNode = aNodes.pop();
    bNode = bNodes.pop();
  }
  return sharedNode;
}
fnord3.layout.chord = function() {
  var chord = {},
      chords,
      groups,
      matrix,
      n,
      padding = 0,
      sortGroups,
      sortSubgroups,
      sortChords;

  function relayout() {
    var subgroups = {},
        groupSums = [],
        groupIndex = fnord3.range(n),
        subgroupIndex = [],
        k,
        x,
        x0,
        i,
        j;

    chords = [];
    groups = [];

    // Compute the sum.
    k = 0, i = -1; while (++i < n) {
      x = 0, j = -1; while (++j < n) {
        x += matrix[i][j];
      }
      groupSums.push(x);
      subgroupIndex.push(fnord3.range(n));
      k += x;
    }

    // Sort groups…
    if (sortGroups) {
      groupIndex.sort(function(a, b) {
        return sortGroups(groupSums[a], groupSums[b]);
      });
    }

    // Sort subgroups…
    if (sortSubgroups) {
      subgroupIndex.forEach(function(d, i) {
        d.sort(function(a, b) {
          return sortSubgroups(matrix[i][a], matrix[i][b]);
        });
      });
    }

    // Convert the sum to scaling factor for [0, 2pi].
    // TODO Allow start and end angle to be specified.
    // TODO Allow padding to be specified as percentage?
    k = (2 * Math.PI - padding * n) / k;

    // Compute the start and end angle for each group and subgroup.
    // Note: Opera has a bug reordering object literal properties!
    x = 0, i = -1; while (++i < n) {
      x0 = x, j = -1; while (++j < n) {
        var di = groupIndex[i],
            dj = subgroupIndex[di][j],
            v = matrix[di][dj],
            a0 = x,
            a1 = x += v * k;
        subgroups[di + "-" + dj] = {
          index: di,
          subindex: dj,
          startAngle: a0,
          endAngle: a1,
          value: v
        };
      }
      groups[di] = {
        index: di,
        startAngle: x0,
        endAngle: x,
        value: (x - x0) / k
      };
      x += padding;
    }

    // Generate chords for each (non-empty) subgroup-subgroup link.
    i = -1; while (++i < n) {
      j = i - 1; while (++j < n) {
        var source = subgroups[i + "-" + j],
            target = subgroups[j + "-" + i];
        if (source.value || target.value) {
          chords.push(source.value < target.value
              ? {source: target, target: source}
              : {source: source, target: target});
        }
      }
    }

    if (sortChords) resort();
  }

  function resort() {
    chords.sort(function(a, b) {
      return sortChords(
          (a.source.value + a.target.value) / 2,
          (b.source.value + b.target.value) / 2);
    });
  }

  chord.matrix = function(x) {
    if (!arguments.length) return matrix;
    n = (matrix = x) && matrix.length;
    chords = groups = null;
    return chord;
  };

  chord.padding = function(x) {
    if (!arguments.length) return padding;
    padding = x;
    chords = groups = null;
    return chord;
  };

  chord.sortGroups = function(x) {
    if (!arguments.length) return sortGroups;
    sortGroups = x;
    chords = groups = null;
    return chord;
  };

  chord.sortSubgroups = function(x) {
    if (!arguments.length) return sortSubgroups;
    sortSubgroups = x;
    chords = null;
    return chord;
  };

  chord.sortChords = function(x) {
    if (!arguments.length) return sortChords;
    sortChords = x;
    if (chords) resort();
    return chord;
  };

  chord.chords = function() {
    if (!chords) relayout();
    return chords;
  };

  chord.groups = function() {
    if (!groups) relayout();
    return groups;
  };

  return chord;
};
// A rudimentary force layout using Gauss-Seidel.
fnord3.layout.force = function() {
  var force = {},
      event = fnord3.dispatch("start", "tick", "end"),
      size = [1, 1],
      drag,
      alpha,
      friction = .9,
      linkDistance = fnord3_layout_forceLinkDistance,
      linkStrength = fnord3_layout_forceLinkStrength,
      charge = -30,
      gravity = .1,
      theta = .8,
      interval,
      nodes = [],
      links = [],
      distances,
      strengths,
      charges;

  function repulse(node) {
    return function(quad, x1, y1, x2, y2) {
      if (quad.point !== node) {
        var dx = quad.cx - node.x,
            dy = quad.cy - node.y,
            dn = 1 / Math.sqrt(dx * dx + dy * dy);

        /* Barnes-Hut criterion. */
        if ((x2 - x1) * dn < theta) {
          var k = quad.charge * dn * dn;
          node.px -= dx * k;
          node.py -= dy * k;
          return true;
        }

        if (quad.point && isFinite(dn)) {
          var k = quad.pointCharge * dn * dn;
          node.px -= dx * k;
          node.py -= dy * k;
        }
      }
      return !quad.charge;
    };
  }

  force.tick = function() {
    // simulated annealing, basically
    if ((alpha *= .99) < .005) {
      event.end({type: "end", alpha: alpha = 0});
      return true;
    }

    var n = nodes.length,
        m = links.length,
        q,
        i, // current index
        o, // current object
        s, // current source
        t, // current target
        l, // current distance
        k, // current force
        x, // x-distance
        y; // y-distance

    // gauss-seidel relaxation for links
    for (i = 0; i < m; ++i) {
      o = links[i];
      s = o.source;
      t = o.target;
      x = t.x - s.x;
      y = t.y - s.y;
      if (l = (x * x + y * y)) {
        l = alpha * strengths[i] * ((l = Math.sqrt(l)) - distances[i]) / l;
        x *= l;
        y *= l;
        t.x -= x * (k = s.weight / (t.weight + s.weight));
        t.y -= y * k;
        s.x += x * (k = 1 - k);
        s.y += y * k;
      }
    }

    // apply gravity forces
    if (k = alpha * gravity) {
      x = size[0] / 2;
      y = size[1] / 2;
      i = -1; if (k) while (++i < n) {
        o = nodes[i];
        o.x += (x - o.x) * k;
        o.y += (y - o.y) * k;
      }
    }

    // compute quadtree center of mass and apply charge forces
    if (charge) {
      fnord3_layout_forceAccumulate(q = fnord3.geom.quadtree(nodes), alpha, charges);
      i = -1; while (++i < n) {
        if (!(o = nodes[i]).fixed) {
          q.visit(repulse(o));
        }
      }
    }

    // position verlet integration
    i = -1; while (++i < n) {
      o = nodes[i];
      if (o.fixed) {
        o.x = o.px;
        o.y = o.py;
      } else {
        o.x -= (o.px - (o.px = o.x)) * friction;
        o.y -= (o.py - (o.py = o.y)) * friction;
      }
    }

    event.tick({type: "tick", alpha: alpha});
  };

  force.nodes = function(x) {
    if (!arguments.length) return nodes;
    nodes = x;
    return force;
  };

  force.links = function(x) {
    if (!arguments.length) return links;
    links = x;
    return force;
  };

  force.size = function(x) {
    if (!arguments.length) return size;
    size = x;
    return force;
  };

  force.linkDistance = function(x) {
    if (!arguments.length) return linkDistance;
    linkDistance = fnord3_functor(x);
    return force;
  };

  // For backwards-compatibility.
  force.distance = force.linkDistance;

  force.linkStrength = function(x) {
    if (!arguments.length) return linkStrength;
    linkStrength = fnord3_functor(x);
    return force;
  };

  force.friction = function(x) {
    if (!arguments.length) return friction;
    friction = x;
    return force;
  };

  force.charge = function(x) {
    if (!arguments.length) return charge;
    charge = typeof x === "function" ? x : +x;
    return force;
  };

  force.gravity = function(x) {
    if (!arguments.length) return gravity;
    gravity = x;
    return force;
  };

  force.theta = function(x) {
    if (!arguments.length) return theta;
    theta = x;
    return force;
  };

  force.alpha = function(x) {
    if (!arguments.length) return alpha;

    if (alpha) { // if we're already running
      if (x > 0) alpha = x; // we might keep it hot
      else alpha = 0; // or, next tick will dispatch "end"
    } else if (x > 0) { // otherwise, fire it up!
      event.start({type: "start", alpha: alpha = x});
      fnord3.timer(force.tick);
    }

    return force;
  };

  force.start = function() {
    var i,
        j,
        n = nodes.length,
        m = links.length,
        w = size[0],
        h = size[1],
        neighbors,
        o;

    for (i = 0; i < n; ++i) {
      (o = nodes[i]).index = i;
      o.weight = 0;
    }

    distances = [];
    strengths = [];
    for (i = 0; i < m; ++i) {
      o = links[i];
      if (typeof o.source == "number") o.source = nodes[o.source];
      if (typeof o.target == "number") o.target = nodes[o.target];
      distances[i] = linkDistance.call(this, o, i);
      strengths[i] = linkStrength.call(this, o, i);
      ++o.source.weight;
      ++o.target.weight;
    }

    for (i = 0; i < n; ++i) {
      o = nodes[i];
      if (isNaN(o.x)) o.x = position("x", w);
      if (isNaN(o.y)) o.y = position("y", h);
      if (isNaN(o.px)) o.px = o.x;
      if (isNaN(o.py)) o.py = o.y;
    }

    charges = [];
    if (typeof charge === "function") {
      for (i = 0; i < n; ++i) {
        charges[i] = +charge.call(this, nodes[i], i);
      }
    } else {
      for (i = 0; i < n; ++i) {
        charges[i] = charge;
      }
    }

    // initialize node position based on first neighbor
    function position(dimension, size) {
      var neighbors = neighbor(i),
          j = -1,
          m = neighbors.length,
          x;
      while (++j < m) if (!isNaN(x = neighbors[j][dimension])) return x;
      return Math.random() * size;
    }

    // initialize neighbors lazily
    function neighbor() {
      if (!neighbors) {
        neighbors = [];
        for (j = 0; j < n; ++j) {
          neighbors[j] = [];
        }
        for (j = 0; j < m; ++j) {
          var o = links[j];
          neighbors[o.source.index].push(o.target);
          neighbors[o.target.index].push(o.source);
        }
      }
      return neighbors[i];
    }

    return force.resume();
  };

  force.resume = function() {
    return force.alpha(.1);
  };

  force.stop = function() {
    return force.alpha(0);
  };

  // use `node.call(force.drag)` to make nodes draggable
  force.drag = function() {
    if (!drag) drag = fnord3.behavior.drag()
        .origin(fnord3_identity)
        .on("dragstart", dragstart)
        .on("drag", fnord3_layout_forceDrag)
        .on("dragend", fnord3_layout_forceDragEnd);

    this.on("mouseover.force", fnord3_layout_forceDragOver)
        .on("mouseout.force", fnord3_layout_forceDragOut)
        .call(drag);
  };

  function dragstart(d) {
    fnord3_layout_forceDragOver(fnord3_layout_forceDragNode = d);
    fnord3_layout_forceDragForce = force;
  }

  return fnord3.rebind(force, event, "on");
};

var fnord3_layout_forceDragForce,
    fnord3_layout_forceDragNode;

function fnord3_layout_forceDragOver(d) {
  d.fixed |= 2;
}

function fnord3_layout_forceDragOut(d) {
  if (d !== fnord3_layout_forceDragNode) d.fixed &= 1;
}

function fnord3_layout_forceDragEnd() {
  fnord3_layout_forceDragNode.fixed &= 1;
  fnord3_layout_forceDragForce = fnord3_layout_forceDragNode = null;
}

function fnord3_layout_forceDrag() {
  fnord3_layout_forceDragNode.px = fnord3.event.x;
  fnord3_layout_forceDragNode.py = fnord3.event.y;
  fnord3_layout_forceDragForce.resume(); // restart annealing
}

function fnord3_layout_forceAccumulate(quad, alpha, charges) {
  var cx = 0,
      cy = 0;
  quad.charge = 0;
  if (!quad.leaf) {
    var nodes = quad.nodes,
        n = nodes.length,
        i = -1,
        c;
    while (++i < n) {
      c = nodes[i];
      if (c == null) continue;
      fnord3_layout_forceAccumulate(c, alpha, charges);
      quad.charge += c.charge;
      cx += c.charge * c.cx;
      cy += c.charge * c.cy;
    }
  }
  if (quad.point) {
    // jitter internal nodes that are coincident
    if (!quad.leaf) {
      quad.point.x += Math.random() - .5;
      quad.point.y += Math.random() - .5;
    }
    var k = alpha * charges[quad.point.index];
    quad.charge += quad.pointCharge = k;
    cx += k * quad.point.x;
    cy += k * quad.point.y;
  }
  quad.cx = cx / quad.charge;
  quad.cy = cy / quad.charge;
}

function fnord3_layout_forceLinkDistance(link) {
  return 20;
}

function fnord3_layout_forceLinkStrength(link) {
  return 1;
}
fnord3.layout.partition = function() {
  var hierarchy = fnord3.layout.hierarchy(),
      size = [1, 1]; // width, height

  function position(node, x, dx, dy) {
    var children = node.children;
    node.x = x;
    node.y = node.depth * dy;
    node.dx = dx;
    node.dy = dy;
    if (children && (n = children.length)) {
      var i = -1,
          n,
          c,
          d;
      dx = node.value ? dx / node.value : 0;
      while (++i < n) {
        position(c = children[i], x, d = c.value * dx, dy);
        x += d;
      }
    }
  }

  function depth(node) {
    var children = node.children,
        d = 0;
    if (children && (n = children.length)) {
      var i = -1,
          n;
      while (++i < n) d = Math.max(d, depth(children[i]));
    }
    return 1 + d;
  }

  function partition(d, i) {
    var nodes = hierarchy.call(this, d, i);
    position(nodes[0], 0, size[0], size[1] / depth(nodes[0]));
    return nodes;
  }

  partition.size = function(x) {
    if (!arguments.length) return size;
    size = x;
    return partition;
  };

  return fnord3_layout_hierarchyRebind(partition, hierarchy);
};
fnord3.layout.pie = function() {
  var value = Number,
      sort = fnord3_layout_pieSortByValue,
      startAngle = 0,
      endAngle = 2 * Math.PI;

  function pie(data, i) {

    // Compute the numeric values for each data element.
    var values = data.map(function(d, i) { return +value.call(pie, d, i); });

    // Compute the start angle.
    var a = +(typeof startAngle === "function"
        ? startAngle.apply(this, arguments)
        : startAngle);

    // Compute the angular scale factor: from value to radians.
    var k = ((typeof endAngle === "function"
        ? endAngle.apply(this, arguments)
        : endAngle) - startAngle)
        / fnord3.sum(values);

    // Optionally sort the data.
    var index = fnord3.range(data.length);
    if (sort != null) index.sort(sort === fnord3_layout_pieSortByValue
        ? function(i, j) { return values[j] - values[i]; }
        : function(i, j) { return sort(data[i], data[j]); });

    // Compute the arcs!
    // They are stored in the original data's order.
    var arcs = [];
    index.forEach(function(i) {
      arcs[i] = {
        data: data[i],
        value: d = values[i],
        startAngle: a,
        endAngle: a += d * k
      };
    });
    return arcs;
  }

  /**
   * Specifies the value function *x*, which returns a nonnegative numeric value
   * for each datum. The default value function is `Number`. The value function
   * is passed two arguments: the current datum and the current index.
   */
  pie.value = function(x) {
    if (!arguments.length) return value;
    value = x;
    return pie;
  };

  /**
   * Specifies a sort comparison operator *x*. The comparator is passed two data
   * elements from the data array, a and b; it returns a negative value if a is
   * less than b, a positive value if a is greater than b, and zero if a equals
   * b.
   */
  pie.sort = function(x) {
    if (!arguments.length) return sort;
    sort = x;
    return pie;
  };

  /**
   * Specifies the overall start angle of the pie chart. Defaults to 0. The
   * start angle can be specified either as a constant or as a function; in the
   * case of a function, it is evaluated once per array (as opposed to per
   * element).
   */
  pie.startAngle = function(x) {
    if (!arguments.length) return startAngle;
    startAngle = x;
    return pie;
  };

  /**
   * Specifies the overall end angle of the pie chart. Defaults to 2π. The
   * end angle can be specified either as a constant or as a function; in the
   * case of a function, it is evaluated once per array (as opposed to per
   * element).
   */
  pie.endAngle = function(x) {
    if (!arguments.length) return endAngle;
    endAngle = x;
    return pie;
  };

  return pie;
};

var fnord3_layout_pieSortByValue = {};
// data is two-dimensional array of x,y; we populate y0
fnord3.layout.stack = function() {
  var values = fnord3_identity,
      order = fnord3_layout_stackOrderDefault,
      offset = fnord3_layout_stackOffsetZero,
      out = fnord3_layout_stackOut,
      x = fnord3_layout_stackX,
      y = fnord3_layout_stackY;

  function stack(data, index) {

    // Convert series to canonical two-dimensional representation.
    var series = data.map(function(d, i) {
      return values.call(stack, d, i);
    });

    // Convert each series to canonical [[x,y]] representation.
    var points = series.map(function(d, i) {
      return d.map(function(v, i) {
        return [x.call(stack, v, i), y.call(stack, v, i)];
      });
    });

    // Compute the order of series, and permute them.
    var orders = order.call(stack, points, index);
    series = fnord3.permute(series, orders);
    points = fnord3.permute(points, orders);

    // Compute the baseline…
    var offsets = offset.call(stack, points, index);

    // And propagate it to other series.
    var n = series.length,
        m = series[0].length,
        i,
        j,
        o;
    for (j = 0; j < m; ++j) {
      out.call(stack, series[0][j], o = offsets[j], points[0][j][1]);
      for (i = 1; i < n; ++i) {
        out.call(stack, series[i][j], o += points[i - 1][j][1], points[i][j][1]);
      }
    }

    return data;
  }

  stack.values = function(x) {
    if (!arguments.length) return values;
    values = x;
    return stack;
  };

  stack.order = function(x) {
    if (!arguments.length) return order;
    order = typeof x === "function" ? x : fnord3_layout_stackOrders.get(x) || fnord3_layout_stackOrderDefault;
    return stack;
  };

  stack.offset = function(x) {
    if (!arguments.length) return offset;
    offset = typeof x === "function" ? x : fnord3_layout_stackOffsets.get(x) || fnord3_layout_stackOffsetZero;
    return stack;
  };

  stack.x = function(z) {
    if (!arguments.length) return x;
    x = z;
    return stack;
  };

  stack.y = function(z) {
    if (!arguments.length) return y;
    y = z;
    return stack;
  };

  stack.out = function(z) {
    if (!arguments.length) return out;
    out = z;
    return stack;
  };

  return stack;
}

function fnord3_layout_stackX(d) {
  return d.x;
}

function fnord3_layout_stackY(d) {
  return d.y;
}

function fnord3_layout_stackOut(d, y0, y) {
  d.y0 = y0;
  d.y = y;
}

var fnord3_layout_stackOrders = fnord3.map({

  "inside-out": function(data) {
    var n = data.length,
        i,
        j,
        max = data.map(fnord3_layout_stackMaxIndex),
        sums = data.map(fnord3_layout_stackReduceSum),
        index = fnord3.range(n).sort(function(a, b) { return max[a] - max[b]; }),
        top = 0,
        bottom = 0,
        tops = [],
        bottoms = [];
    for (i = 0; i < n; ++i) {
      j = index[i];
      if (top < bottom) {
        top += sums[j];
        tops.push(j);
      } else {
        bottom += sums[j];
        bottoms.push(j);
      }
    }
    return bottoms.reverse().concat(tops);
  },

  "reverse": function(data) {
    return fnord3.range(data.length).reverse();
  },

  "default": fnord3_layout_stackOrderDefault

});

var fnord3_layout_stackOffsets = fnord3.map({

  "silhouette": function(data) {
    var n = data.length,
        m = data[0].length,
        sums = [],
        max = 0,
        i,
        j,
        o,
        y0 = [];
    for (j = 0; j < m; ++j) {
      for (i = 0, o = 0; i < n; i++) o += data[i][j][1];
      if (o > max) max = o;
      sums.push(o);
    }
    for (j = 0; j < m; ++j) {
      y0[j] = (max - sums[j]) / 2;
    }
    return y0;
  },

  "wiggle": function(data) {
    var n = data.length,
        x = data[0],
        m = x.length,
        max = 0,
        i,
        j,
        k,
        s1,
        s2,
        s3,
        dx,
        o,
        o0,
        y0 = [];
    y0[0] = o = o0 = 0;
    for (j = 1; j < m; ++j) {
      for (i = 0, s1 = 0; i < n; ++i) s1 += data[i][j][1];
      for (i = 0, s2 = 0, dx = x[j][0] - x[j - 1][0]; i < n; ++i) {
        for (k = 0, s3 = (data[i][j][1] - data[i][j - 1][1]) / (2 * dx); k < i; ++k) {
          s3 += (data[k][j][1] - data[k][j - 1][1]) / dx;
        }
        s2 += s3 * data[i][j][1];
      }
      y0[j] = o -= s1 ? s2 / s1 * dx : 0;
      if (o < o0) o0 = o;
    }
    for (j = 0; j < m; ++j) y0[j] -= o0;
    return y0;
  },

  "expand": function(data) {
    var n = data.length,
        m = data[0].length,
        k = 1 / n,
        i,
        j,
        o,
        y0 = [];
    for (j = 0; j < m; ++j) {
      for (i = 0, o = 0; i < n; i++) o += data[i][j][1];
      if (o) for (i = 0; i < n; i++) data[i][j][1] /= o;
      else for (i = 0; i < n; i++) data[i][j][1] = k;
    }
    for (j = 0; j < m; ++j) y0[j] = 0;
    return y0;
  },

  "zero": fnord3_layout_stackOffsetZero

});

function fnord3_layout_stackOrderDefault(data) {
  return fnord3.range(data.length);
}

function fnord3_layout_stackOffsetZero(data) {
  var j = -1,
      m = data[0].length,
      y0 = [];
  while (++j < m) y0[j] = 0;
  return y0;
}

function fnord3_layout_stackMaxIndex(array) {
  var i = 1,
      j = 0,
      v = array[0][1],
      k,
      n = array.length;
  for (; i < n; ++i) {
    if ((k = array[i][1]) > v) {
      j = i;
      v = k;
    }
  }
  return j;
}

function fnord3_layout_stackReduceSum(d) {
  return d.reduce(fnord3_layout_stackSum, 0);
}

function fnord3_layout_stackSum(p, d) {
  return p + d[1];
}
fnord3.layout.histogram = function() {
  var frequency = true,
      valuer = Number,
      ranger = fnord3_layout_histogramRange,
      binner = fnord3_layout_histogramBinSturges;

  function histogram(data, i) {
    var bins = [],
        values = data.map(valuer, this),
        range = ranger.call(this, values, i),
        thresholds = binner.call(this, range, values, i),
        bin,
        i = -1,
        n = values.length,
        m = thresholds.length - 1,
        k = frequency ? 1 : 1 / n,
        x;

    // Initialize the bins.
    while (++i < m) {
      bin = bins[i] = [];
      bin.dx = thresholds[i + 1] - (bin.x = thresholds[i]);
      bin.y = 0;
    }

    // Fill the bins, ignoring values outside the range.
    if (m > 0) {
      i = -1; while(++i < n) {
        x = values[i];
        if ((x >= range[0]) && (x <= range[1])) {
          bin = bins[fnord3.bisect(thresholds, x, 1, m) - 1];
          bin.y += k;
          bin.push(data[i]);
        }
      }
    }

    return bins;
  }

  // Specifies how to extract a value from the associated data. The default
  // value function is `Number`, which is equivalent to the identity function.
  histogram.value = function(x) {
    if (!arguments.length) return valuer;
    valuer = x;
    return histogram;
  };

  // Specifies the range of the histogram. Values outside the specified range
  // will be ignored. The argument `x` may be specified either as a two-element
  // array representing the minimum and maximum value of the range, or as a
  // function that returns the range given the array of values and the current
  // index `i`. The default range is the extent (minimum and maximum) of the
  // values.
  histogram.range = function(x) {
    if (!arguments.length) return ranger;
    ranger = fnord3_functor(x);
    return histogram;
  };

  // Specifies how to bin values in the histogram. The argument `x` may be
  // specified as a number, in which case the range of values will be split
  // uniformly into the given number of bins. Or, `x` may be an array of
  // threshold values, defining the bins; the specified array must contain the
  // rightmost (upper) value, thus specifying n + 1 values for n bins. Or, `x`
  // may be a function which is evaluated, being passed the range, the array of
  // values, and the current index `i`, returning an array of thresholds. The
  // default bin function will divide the values into uniform bins using
  // Sturges' formula.
  histogram.bins = function(x) {
    if (!arguments.length) return binner;
    binner = typeof x === "number"
        ? function(range) { return fnord3_layout_histogramBinFixed(range, x); }
        : fnord3_functor(x);
    return histogram;
  };

  // Specifies whether the histogram's `y` value is a count (frequency) or a
  // probability (density). The default value is true.
  histogram.frequency = function(x) {
    if (!arguments.length) return frequency;
    frequency = !!x;
    return histogram;
  };

  return histogram;
};

function fnord3_layout_histogramBinSturges(range, values) {
  return fnord3_layout_histogramBinFixed(range, Math.ceil(Math.log(values.length) / Math.LN2 + 1));
}

function fnord3_layout_histogramBinFixed(range, n) {
  var x = -1,
      b = +range[0],
      m = (range[1] - b) / n,
      f = [];
  while (++x <= n) f[x] = m * x + b;
  return f;
}

function fnord3_layout_histogramRange(values) {
  return [fnord3.min(values), fnord3.max(values)];
}
fnord3.layout.hierarchy = function() {
  var sort = fnord3_layout_hierarchySort,
      children = fnord3_layout_hierarchyChildren,
      value = fnord3_layout_hierarchyValue;

  // Recursively compute the node depth and value.
  // Also converts the data representation into a standard hierarchy structure.
  function recurse(data, depth, nodes) {
    var childs = children.call(hierarchy, data, depth),
        node = fnord3_layout_hierarchyInline ? data : {data: data};
    node.depth = depth;
    nodes.push(node);
    if (childs && (n = childs.length)) {
      var i = -1,
          n,
          c = node.children = [],
          v = 0,
          j = depth + 1;
      while (++i < n) {
        d = recurse(childs[i], j, nodes);
        d.parent = node;
        c.push(d);
        v += d.value;
      }
      if (sort) c.sort(sort);
      if (value) node.value = v;
    } else if (value) {
      node.value = +value.call(hierarchy, data, depth) || 0;
    }
    return node;
  }

  // Recursively re-evaluates the node value.
  function revalue(node, depth) {
    var children = node.children,
        v = 0;
    if (children && (n = children.length)) {
      var i = -1,
          n,
          j = depth + 1;
      while (++i < n) v += revalue(children[i], j);
    } else if (value) {
      v = +value.call(hierarchy, fnord3_layout_hierarchyInline ? node : node.data, depth) || 0;
    }
    if (value) node.value = v;
    return v;
  }

  function hierarchy(d) {
    var nodes = [];
    recurse(d, 0, nodes);
    return nodes;
  }

  hierarchy.sort = function(x) {
    if (!arguments.length) return sort;
    sort = x;
    return hierarchy;
  };

  hierarchy.children = function(x) {
    if (!arguments.length) return children;
    children = x;
    return hierarchy;
  };

  hierarchy.value = function(x) {
    if (!arguments.length) return value;
    value = x;
    return hierarchy;
  };

  // Re-evaluates the `value` property for the specified hierarchy.
  hierarchy.revalue = function(root) {
    revalue(root, 0);
    return root;
  };

  return hierarchy;
};

// A method assignment helper for hierarchy subclasses.
function fnord3_layout_hierarchyRebind(object, hierarchy) {
  fnord3.rebind(object, hierarchy, "sort", "children", "value");

  // Add an alias for links, for convenience.
  object.links = fnord3_layout_hierarchyLinks;

  // If the new API is used, enabling inlining.
  object.nodes = function(d) {
    fnord3_layout_hierarchyInline = true;
    return (object.nodes = object)(d);
  };

  return object;
}

function fnord3_layout_hierarchyChildren(d) {
  return d.children;
}

function fnord3_layout_hierarchyValue(d) {
  return d.value;
}

function fnord3_layout_hierarchySort(a, b) {
  return b.value - a.value;
}

// Returns an array source+target objects for the specified nodes.
function fnord3_layout_hierarchyLinks(nodes) {
  return fnord3.merge(nodes.map(function(parent) {
    return (parent.children || []).map(function(child) {
      return {source: parent, target: child};
    });
  }));
}

// For backwards-compatibility, don't enable inlining by default.
var fnord3_layout_hierarchyInline = false;
fnord3.layout.pack = function() {
  var hierarchy = fnord3.layout.hierarchy().sort(fnord3_layout_packSort),
      size = [1, 1];

  function pack(d, i) {
    var nodes = hierarchy.call(this, d, i),
        root = nodes[0];

    // Recursively compute the layout.
    root.x = 0;
    root.y = 0;
    fnord3_layout_packTree(root);

    // Scale the layout to fit the requested size.
    var w = size[0],
        h = size[1],
        k = 1 / Math.max(2 * root.r / w, 2 * root.r / h);
    fnord3_layout_packTransform(root, w / 2, h / 2, k);

    return nodes;
  }

  pack.size = function(x) {
    if (!arguments.length) return size;
    size = x;
    return pack;
  };

  return fnord3_layout_hierarchyRebind(pack, hierarchy);
};

function fnord3_layout_packSort(a, b) {
  return a.value - b.value;
}

function fnord3_layout_packInsert(a, b) {
  var c = a._pack_next;
  a._pack_next = b;
  b._pack_prev = a;
  b._pack_next = c;
  c._pack_prev = b;
}

function fnord3_layout_packSplice(a, b) {
  a._pack_next = b;
  b._pack_prev = a;
}

function fnord3_layout_packIntersects(a, b) {
  var dx = b.x - a.x,
      dy = b.y - a.y,
      dr = a.r + b.r;
  return dr * dr - dx * dx - dy * dy > .001; // within epsilon
}

function fnord3_layout_packCircle(nodes) {
  var xMin = Infinity,
      xMax = -Infinity,
      yMin = Infinity,
      yMax = -Infinity,
      n = nodes.length,
      a, b, c, j, k;

  function bound(node) {
    xMin = Math.min(node.x - node.r, xMin);
    xMax = Math.max(node.x + node.r, xMax);
    yMin = Math.min(node.y - node.r, yMin);
    yMax = Math.max(node.y + node.r, yMax);
  }

  // Create node links.
  nodes.forEach(fnord3_layout_packLink);

  // Create first node.
  a = nodes[0];
  a.x = -a.r;
  a.y = 0;
  bound(a);

  // Create second node.
  if (n > 1) {
    b = nodes[1];
    b.x = b.r;
    b.y = 0;
    bound(b);

    // Create third node and build chain.
    if (n > 2) {
      c = nodes[2];
      fnord3_layout_packPlace(a, b, c);
      bound(c);
      fnord3_layout_packInsert(a, c);
      a._pack_prev = c;
      fnord3_layout_packInsert(c, b);
      b = a._pack_next;

      // Now iterate through the rest.
      for (var i = 3; i < n; i++) {
        fnord3_layout_packPlace(a, b, c = nodes[i]);

        // Search for the closest intersection.
        var isect = 0, s1 = 1, s2 = 1;
        for (j = b._pack_next; j !== b; j = j._pack_next, s1++) {
          if (fnord3_layout_packIntersects(j, c)) {
            isect = 1;
            break;
          }
        }
        if (isect == 1) {
          for (k = a._pack_prev; k !== j._pack_prev; k = k._pack_prev, s2++) {
            if (fnord3_layout_packIntersects(k, c)) {
              break;
            }
          }
        }

        // Update node chain.
        if (isect) {
          if (s1 < s2 || (s1 == s2 && b.r < a.r)) fnord3_layout_packSplice(a, b = j);
          else fnord3_layout_packSplice(a = k, b);
          i--;
        } else {
          fnord3_layout_packInsert(a, c);
          b = c;
          bound(c);
        }
      }
    }
  }

  // Re-center the circles and return the encompassing radius.
  var cx = (xMin + xMax) / 2,
      cy = (yMin + yMax) / 2,
      cr = 0;
  for (var i = 0; i < n; i++) {
    var node = nodes[i];
    node.x -= cx;
    node.y -= cy;
    cr = Math.max(cr, node.r + Math.sqrt(node.x * node.x + node.y * node.y));
  }

  // Remove node links.
  nodes.forEach(fnord3_layout_packUnlink);

  return cr;
}

function fnord3_layout_packLink(node) {
  node._pack_next = node._pack_prev = node;
}

function fnord3_layout_packUnlink(node) {
  delete node._pack_next;
  delete node._pack_prev;
}

function fnord3_layout_packTree(node) {
  var children = node.children;
  if (children && children.length) {
    children.forEach(fnord3_layout_packTree);
    node.r = fnord3_layout_packCircle(children);
  } else {
    node.r = Math.sqrt(node.value);
  }
}

function fnord3_layout_packTransform(node, x, y, k) {
  var children = node.children;
  node.x = (x += k * node.x);
  node.y = (y += k * node.y);
  node.r *= k;
  if (children) {
    var i = -1, n = children.length;
    while (++i < n) fnord3_layout_packTransform(children[i], x, y, k);
  }
}

function fnord3_layout_packPlace(a, b, c) {
  var db = a.r + c.r,
      dx = b.x - a.x,
      dy = b.y - a.y;
  if (db && (dx || dy)) {
    var da = b.r + c.r,
        dc = Math.sqrt(dx * dx + dy * dy),
        cos = Math.max(-1, Math.min(1, (db * db + dc * dc - da * da) / (2 * db * dc))),
        theta = Math.acos(cos),
        x = cos * (db /= dc),
        y = Math.sin(theta) * db;
    c.x = a.x + x * dx + y * dy;
    c.y = a.y + x * dy - y * dx;
  } else {
    c.x = a.x + db;
    c.y = a.y;
  }
}
// Implements a hierarchical layout using the cluster (or dendrogram)
// algorithm.
fnord3.layout.cluster = function() {
  var hierarchy = fnord3.layout.hierarchy().sort(null).value(null),
      separation = fnord3_layout_treeSeparation,
      size = [1, 1]; // width, height

  function cluster(d, i) {
    var nodes = hierarchy.call(this, d, i),
        root = nodes[0],
        previousNode,
        x = 0,
        kx,
        ky;

    // First walk, computing the initial x & y values.
    fnord3_layout_treeVisitAfter(root, function(node) {
      var children = node.children;
      if (children && children.length) {
        node.x = fnord3_layout_clusterX(children);
        node.y = fnord3_layout_clusterY(children);
      } else {
        node.x = previousNode ? x += separation(node, previousNode) : 0;
        node.y = 0;
        previousNode = node;
      }
    });

    // Compute the left-most, right-most, and depth-most nodes for extents.
    var left = fnord3_layout_clusterLeft(root),
        right = fnord3_layout_clusterRight(root),
        x0 = left.x - separation(left, right) / 2,
        x1 = right.x + separation(right, left) / 2;

    // Second walk, normalizing x & y to the desired size.
    fnord3_layout_treeVisitAfter(root, function(node) {
      node.x = (node.x - x0) / (x1 - x0) * size[0];
      node.y = (1 - (root.y ? node.y / root.y : 1)) * size[1];
    });

    return nodes;
  }

  cluster.separation = function(x) {
    if (!arguments.length) return separation;
    separation = x;
    return cluster;
  };

  cluster.size = function(x) {
    if (!arguments.length) return size;
    size = x;
    return cluster;
  };

  return fnord3_layout_hierarchyRebind(cluster, hierarchy);
};

function fnord3_layout_clusterY(children) {
  return 1 + fnord3.max(children, function(child) {
    return child.y;
  });
}

function fnord3_layout_clusterX(children) {
  return children.reduce(function(x, child) {
    return x + child.x;
  }, 0) / children.length;
}

function fnord3_layout_clusterLeft(node) {
  var children = node.children;
  return children && children.length ? fnord3_layout_clusterLeft(children[0]) : node;
}

function fnord3_layout_clusterRight(node) {
  var children = node.children, n;
  return children && (n = children.length) ? fnord3_layout_clusterRight(children[n - 1]) : node;
}
// Node-link tree diagram using the Reingold-Tilford "tidy" algorithm
fnord3.layout.tree = function() {
  var hierarchy = fnord3.layout.hierarchy().sort(null).value(null),
      separation = fnord3_layout_treeSeparation,
      size = [1, 1]; // width, height

  function tree(d, i) {
    var nodes = hierarchy.call(this, d, i),
        root = nodes[0];

    function firstWalk(node, previousSibling) {
      var children = node.children,
          layout = node._tree;
      if (children && (n = children.length)) {
        var n,
            firstChild = children[0],
            previousChild,
            ancestor = firstChild,
            child,
            i = -1;
        while (++i < n) {
          child = children[i];
          firstWalk(child, previousChild);
          ancestor = apportion(child, previousChild, ancestor);
          previousChild = child;
        }
        fnord3_layout_treeShift(node);
        var midpoint = .5 * (firstChild._tree.prelim + child._tree.prelim);
        if (previousSibling) {
          layout.prelim = previousSibling._tree.prelim + separation(node, previousSibling);
          layout.mod = layout.prelim - midpoint;
        } else {
          layout.prelim = midpoint;
        }
      } else {
        if (previousSibling) {
          layout.prelim = previousSibling._tree.prelim + separation(node, previousSibling);
        }
      }
    }

    function secondWalk(node, x) {
      node.x = node._tree.prelim + x;
      var children = node.children;
      if (children && (n = children.length)) {
        var i = -1,
            n;
        x += node._tree.mod;
        while (++i < n) {
          secondWalk(children[i], x);
        }
      }
    }

    function apportion(node, previousSibling, ancestor) {
      if (previousSibling) {
        var vip = node,
            vop = node,
            vim = previousSibling,
            vom = node.parent.children[0],
            sip = vip._tree.mod,
            sop = vop._tree.mod,
            sim = vim._tree.mod,
            som = vom._tree.mod,
            shift;
        while (vim = fnord3_layout_treeRight(vim), vip = fnord3_layout_treeLeft(vip), vim && vip) {
          vom = fnord3_layout_treeLeft(vom);
          vop = fnord3_layout_treeRight(vop);
          vop._tree.ancestor = node;
          shift = vim._tree.prelim + sim - vip._tree.prelim - sip + separation(vim, vip);
          if (shift > 0) {
            fnord3_layout_treeMove(fnord3_layout_treeAncestor(vim, node, ancestor), node, shift);
            sip += shift;
            sop += shift;
          }
          sim += vim._tree.mod;
          sip += vip._tree.mod;
          som += vom._tree.mod;
          sop += vop._tree.mod;
        }
        if (vim && !fnord3_layout_treeRight(vop)) {
          vop._tree.thread = vim;
          vop._tree.mod += sim - sop;
        }
        if (vip && !fnord3_layout_treeLeft(vom)) {
          vom._tree.thread = vip;
          vom._tree.mod += sip - som;
          ancestor = node;
        }
      }
      return ancestor;
    }

    // Initialize temporary layout variables.
    fnord3_layout_treeVisitAfter(root, function(node, previousSibling) {
      node._tree = {
        ancestor: node,
        prelim: 0,
        mod: 0,
        change: 0,
        shift: 0,
        number: previousSibling ? previousSibling._tree.number + 1 : 0
      };
    });

    // Compute the layout using Buchheim et al.'s algorithm.
    firstWalk(root);
    secondWalk(root, -root._tree.prelim);

    // Compute the left-most, right-most, and depth-most nodes for extents.
    var left = fnord3_layout_treeSearch(root, fnord3_layout_treeLeftmost),
        right = fnord3_layout_treeSearch(root, fnord3_layout_treeRightmost),
        deep = fnord3_layout_treeSearch(root, fnord3_layout_treeDeepest),
        x0 = left.x - separation(left, right) / 2,
        x1 = right.x + separation(right, left) / 2,
        y1 = deep.depth || 1;

    // Clear temporary layout variables; transform x and y.
    fnord3_layout_treeVisitAfter(root, function(node) {
      node.x = (node.x - x0) / (x1 - x0) * size[0];
      node.y = node.depth / y1 * size[1];
      delete node._tree;
    });

    return nodes;
  }

  tree.separation = function(x) {
    if (!arguments.length) return separation;
    separation = x;
    return tree;
  };

  tree.size = function(x) {
    if (!arguments.length) return size;
    size = x;
    return tree;
  };

  return fnord3_layout_hierarchyRebind(tree, hierarchy);
};

function fnord3_layout_treeSeparation(a, b) {
  return a.parent == b.parent ? 1 : 2;
}

// function fnord3_layout_treeSeparationRadial(a, b) {
//   return (a.parent == b.parent ? 1 : 2) / a.depth;
// }

function fnord3_layout_treeLeft(node) {
  var children = node.children;
  return children && children.length ? children[0] : node._tree.thread;
}

function fnord3_layout_treeRight(node) {
  var children = node.children,
      n;
  return children && (n = children.length) ? children[n - 1] : node._tree.thread;
}

function fnord3_layout_treeSearch(node, compare) {
  var children = node.children;
  if (children && (n = children.length)) {
    var child,
        n,
        i = -1;
    while (++i < n) {
      if (compare(child = fnord3_layout_treeSearch(children[i], compare), node) > 0) {
        node = child;
      }
    }
  }
  return node;
}

function fnord3_layout_treeRightmost(a, b) {
  return a.x - b.x;
}

function fnord3_layout_treeLeftmost(a, b) {
  return b.x - a.x;
}

function fnord3_layout_treeDeepest(a, b) {
  return a.depth - b.depth;
}

function fnord3_layout_treeVisitAfter(node, callback) {
  function visit(node, previousSibling) {
    var children = node.children;
    if (children && (n = children.length)) {
      var child,
          previousChild = null,
          i = -1,
          n;
      while (++i < n) {
        child = children[i];
        visit(child, previousChild);
        previousChild = child;
      }
    }
    callback(node, previousSibling);
  }
  visit(node, null);
}

function fnord3_layout_treeShift(node) {
  var shift = 0,
      change = 0,
      children = node.children,
      i = children.length,
      child;
  while (--i >= 0) {
    child = children[i]._tree;
    child.prelim += shift;
    child.mod += shift;
    shift += child.shift + (change += child.change);
  }
}

function fnord3_layout_treeMove(ancestor, node, shift) {
  ancestor = ancestor._tree;
  node = node._tree;
  var change = shift / (node.number - ancestor.number);
  ancestor.change += change;
  node.change -= change;
  node.shift += shift;
  node.prelim += shift;
  node.mod += shift;
}

function fnord3_layout_treeAncestor(vim, node, ancestor) {
  return vim._tree.ancestor.parent == node.parent
      ? vim._tree.ancestor
      : ancestor;
}
// Squarified Treemaps by Mark Bruls, Kees Huizing, and Jarke J. van Wijk
// Modified to support a target aspect ratio by Jeff Heer
fnord3.layout.treemap = function() {
  var hierarchy = fnord3.layout.hierarchy(),
      round = Math.round,
      size = [1, 1], // width, height
      padding = null,
      pad = fnord3_layout_treemapPadNull,
      sticky = false,
      stickies,
      ratio = 0.5 * (1 + Math.sqrt(5)); // golden ratio

  // Compute the area for each child based on value & scale.
  function scale(children, k) {
    var i = -1,
        n = children.length,
        child,
        area;
    while (++i < n) {
      area = (child = children[i]).value * (k < 0 ? 0 : k);
      child.area = isNaN(area) || area <= 0 ? 0 : area;
    }
  }

  // Recursively arranges the specified node's children into squarified rows.
  function squarify(node) {
    var children = node.children;
    if (children && children.length) {
      var rect = pad(node),
          row = [],
          remaining = children.slice(), // copy-on-write
          child,
          best = Infinity, // the best row score so far
          score, // the current row score
          u = Math.min(rect.dx, rect.dy), // initial orientation
          n;
      scale(remaining, rect.dx * rect.dy / node.value);
      row.area = 0;
      while ((n = remaining.length) > 0) {
        row.push(child = remaining[n - 1]);
        row.area += child.area;
        if ((score = worst(row, u)) <= best) { // continue with this orientation
          remaining.pop();
          best = score;
        } else { // abort, and try a different orientation
          row.area -= row.pop().area;
          position(row, u, rect, false);
          u = Math.min(rect.dx, rect.dy);
          row.length = row.area = 0;
          best = Infinity;
        }
      }
      if (row.length) {
        position(row, u, rect, true);
        row.length = row.area = 0;
      }
      children.forEach(squarify);
    }
  }

  // Recursively resizes the specified node's children into existing rows.
  // Preserves the existing layout!
  function stickify(node) {
    var children = node.children;
    if (children && children.length) {
      var rect = pad(node),
          remaining = children.slice(), // copy-on-write
          child,
          row = [];
      scale(remaining, rect.dx * rect.dy / node.value);
      row.area = 0;
      while (child = remaining.pop()) {
        row.push(child);
        row.area += child.area;
        if (child.z != null) {
          position(row, child.z ? rect.dx : rect.dy, rect, !remaining.length);
          row.length = row.area = 0;
        }
      }
      children.forEach(stickify);
    }
  }

  // Computes the score for the specified row, as the worst aspect ratio.
  function worst(row, u) {
    var s = row.area,
        r,
        rmax = 0,
        rmin = Infinity,
        i = -1,
        n = row.length;
    while (++i < n) {
      if (!(r = row[i].area)) continue;
      if (r < rmin) rmin = r;
      if (r > rmax) rmax = r;
    }
    s *= s;
    u *= u;
    return s
        ? Math.max((u * rmax * ratio) / s, s / (u * rmin * ratio))
        : Infinity;
  }

  // Positions the specified row of nodes. Modifies `rect`.
  function position(row, u, rect, flush) {
    var i = -1,
        n = row.length,
        x = rect.x,
        y = rect.y,
        v = u ? round(row.area / u) : 0,
        o;
    if (u == rect.dx) { // horizontal subdivision
      if (flush || v > rect.dy) v = rect.dy; // over+underflow
      while (++i < n) {
        o = row[i];
        o.x = x;
        o.y = y;
        o.dy = v;
        x += o.dx = Math.min(rect.x + rect.dx - x, v ? round(o.area / v) : 0);
      }
      o.z = true;
      o.dx += rect.x + rect.dx - x; // rounding error
      rect.y += v;
      rect.dy -= v;
    } else { // vertical subdivision
      if (flush || v > rect.dx) v = rect.dx; // over+underflow
      while (++i < n) {
        o = row[i];
        o.x = x;
        o.y = y;
        o.dx = v;
        y += o.dy = Math.min(rect.y + rect.dy - y, v ? round(o.area / v) : 0);
      }
      o.z = false;
      o.dy += rect.y + rect.dy - y; // rounding error
      rect.x += v;
      rect.dx -= v;
    }
  }

  function treemap(d) {
    var nodes = stickies || hierarchy(d),
        root = nodes[0];
    root.x = 0;
    root.y = 0;
    root.dx = size[0];
    root.dy = size[1];
    if (stickies) hierarchy.revalue(root);
    scale([root], root.dx * root.dy / root.value);
    (stickies ? stickify : squarify)(root);
    if (sticky) stickies = nodes;
    return nodes;
  }

  treemap.size = function(x) {
    if (!arguments.length) return size;
    size = x;
    return treemap;
  };

  treemap.padding = function(x) {
    if (!arguments.length) return padding;

    function padFunction(node) {
      var p = x.call(treemap, node, node.depth);
      return p == null
          ? fnord3_layout_treemapPadNull(node)
          : fnord3_layout_treemapPad(node, typeof p === "number" ? [p, p, p, p] : p);
    }

    function padConstant(node) {
      return fnord3_layout_treemapPad(node, x);
    }

    var type;
    pad = (padding = x) == null ? fnord3_layout_treemapPadNull
        : (type = typeof x) === "function" ? padFunction
        : type === "number" ? (x = [x, x, x, x], padConstant)
        : padConstant;
    return treemap;
  };

  treemap.round = function(x) {
    if (!arguments.length) return round != Number;
    round = x ? Math.round : Number;
    return treemap;
  };

  treemap.sticky = function(x) {
    if (!arguments.length) return sticky;
    sticky = x;
    stickies = null;
    return treemap;
  };

  treemap.ratio = function(x) {
    if (!arguments.length) return ratio;
    ratio = x;
    return treemap;
  };

  return fnord3_layout_hierarchyRebind(treemap, hierarchy);
};

function fnord3_layout_treemapPadNull(node) {
  return {x: node.x, y: node.y, dx: node.dx, dy: node.dy};
}

function fnord3_layout_treemapPad(node, padding) {
  var x = node.x + padding[3],
      y = node.y + padding[0],
      dx = node.dx - padding[1] - padding[3],
      dy = node.dy - padding[0] - padding[2];
  if (dx < 0) { x += dx / 2; dx = 0; }
  if (dy < 0) { y += dy / 2; dy = 0; }
  return {x: x, y: y, dx: dx, dy: dy};
}
fnord3.csv = function(url, callback) {
  fnord3.text(url, "text/csv", function(text) {
    callback(text && fnord3.csv.parse(text));
  });
};
fnord3.csv.parse = function(text) {
  var header;
  return fnord3.csv.parseRows(text, function(row, i) {
    if (i) {
      var o = {}, j = -1, m = header.length;
      while (++j < m) o[header[j]] = row[j];
      return o;
    } else {
      header = row;
      return null;
    }
  });
};

fnord3.csv.parseRows = function(text, f) {
  var EOL = {}, // sentinel value for end-of-line
      EOF = {}, // sentinel value for end-of-file
      rows = [], // output rows
      re = /\r\n|[,\r\n]/g, // field separator regex
      n = 0, // the current line number
      t, // the current token
      eol; // is the current token followed by EOL?

  re.lastIndex = 0; // work-around bug in FF 3.6

  /** @private Returns the next token. */
  function token() {
    if (re.lastIndex >= text.length) return EOF; // special case: end of file
    if (eol) { eol = false; return EOL; } // special case: end of line

    // special case: quotes
    var j = re.lastIndex;
    if (text.charCodeAt(j) === 34) {
      var i = j;
      while (i++ < text.length) {
        if (text.charCodeAt(i) === 34) {
          if (text.charCodeAt(i + 1) !== 34) break;
          i++;
        }
      }
      re.lastIndex = i + 2;
      var c = text.charCodeAt(i + 1);
      if (c === 13) {
        eol = true;
        if (text.charCodeAt(i + 2) === 10) re.lastIndex++;
      } else if (c === 10) {
        eol = true;
      }
      return text.substring(j + 1, i).replace(/""/g, "\"");
    }

    // common case
    var m = re.exec(text);
    if (m) {
      eol = m[0].charCodeAt(0) !== 44;
      return text.substring(j, m.index);
    }
    re.lastIndex = text.length;
    return text.substring(j);
  }

  while ((t = token()) !== EOF) {
    var a = [];
    while ((t !== EOL) && (t !== EOF)) {
      a.push(t);
      t = token();
    }
    if (f && !(a = f(a, n++))) continue;
    rows.push(a);
  }

  return rows;
};
fnord3.csv.format = function(rows) {
  return rows.map(fnord3_csv_formatRow).join("\n");
};

function fnord3_csv_formatRow(row) {
  return row.map(fnord3_csv_formatValue).join(",");
}

function fnord3_csv_formatValue(text) {
  return /[",\n]/.test(text)
      ? "\"" + text.replace(/\"/g, "\"\"") + "\""
      : text;
}
fnord3.geo = {};

var fnord3_geo_radians = Math.PI / 180;
// TODO clip input coordinates on opposite hemisphere
fnord3.geo.azimuthal = function() {
  var mode = "orthographic", // or stereographic, gnomonic, equidistant or equalarea
      origin,
      scale = 200,
      translate = [480, 250],
      x0,
      y0,
      cy0,
      sy0;

  function azimuthal(coordinates) {
    var x1 = coordinates[0] * fnord3_geo_radians - x0,
        y1 = coordinates[1] * fnord3_geo_radians,
        cx1 = Math.cos(x1),
        sx1 = Math.sin(x1),
        cy1 = Math.cos(y1),
        sy1 = Math.sin(y1),
        cc = mode !== "orthographic" ? sy0 * sy1 + cy0 * cy1 * cx1 : null,
        c,
        k = mode === "stereographic" ? 1 / (1 + cc)
          : mode === "gnomonic" ? 1 / cc
          : mode === "equidistant" ? (c = Math.acos(cc), c ? c / Math.sin(c) : 0)
          : mode === "equalarea" ? Math.sqrt(2 / (1 + cc))
          : 1,
        x = k * cy1 * sx1,
        y = k * (sy0 * cy1 * cx1 - cy0 * sy1);
    return [
      scale * x + translate[0],
      scale * y + translate[1]
    ];
  }

  azimuthal.invert = function(coordinates) {
    var x = (coordinates[0] - translate[0]) / scale,
        y = (coordinates[1] - translate[1]) / scale,
        p = Math.sqrt(x * x + y * y),
        c = mode === "stereographic" ? 2 * Math.atan(p)
          : mode === "gnomonic" ? Math.atan(p)
          : mode === "equidistant" ? p
          : mode === "equalarea" ? 2 * Math.asin(.5 * p)
          : Math.asin(p),
        sc = Math.sin(c),
        cc = Math.cos(c);
    return [
      (x0 + Math.atan2(x * sc, p * cy0 * cc + y * sy0 * sc)) / fnord3_geo_radians,
      Math.asin(cc * sy0 - (p ? (y * sc * cy0) / p : 0)) / fnord3_geo_radians
    ];
  };

  azimuthal.mode = function(x) {
    if (!arguments.length) return mode;
    mode = x + "";
    return azimuthal;
  };

  azimuthal.origin = function(x) {
    if (!arguments.length) return origin;
    origin = x;
    x0 = origin[0] * fnord3_geo_radians;
    y0 = origin[1] * fnord3_geo_radians;
    cy0 = Math.cos(y0);
    sy0 = Math.sin(y0);
    return azimuthal;
  };

  azimuthal.scale = function(x) {
    if (!arguments.length) return scale;
    scale = +x;
    return azimuthal;
  };

  azimuthal.translate = function(x) {
    if (!arguments.length) return translate;
    translate = [+x[0], +x[1]];
    return azimuthal;
  };

  return azimuthal.origin([0, 0]);
};
// Derived from Tom Carden's Albers implementation for Protovis.
// http://gist.github.com/476238
// http://mathworld.wolfram.com/AlbersEqual-AreaConicProjection.html

fnord3.geo.albers = function() {
  var origin = [-98, 38],
      parallels = [29.5, 45.5],
      scale = 1000,
      translate = [480, 250],
      lng0, // fnord3_geo_radians * origin[0]
      n,
      C,
      p0;

  function albers(coordinates) {
    var t = n * (fnord3_geo_radians * coordinates[0] - lng0),
        p = Math.sqrt(C - 2 * n * Math.sin(fnord3_geo_radians * coordinates[1])) / n;
    return [
      scale * p * Math.sin(t) + translate[0],
      scale * (p * Math.cos(t) - p0) + translate[1]
    ];
  }

  albers.invert = function(coordinates) {
    var x = (coordinates[0] - translate[0]) / scale,
        y = (coordinates[1] - translate[1]) / scale,
        p0y = p0 + y,
        t = Math.atan2(x, p0y),
        p = Math.sqrt(x * x + p0y * p0y);
    return [
      (lng0 + t / n) / fnord3_geo_radians,
      Math.asin((C - p * p * n * n) / (2 * n)) / fnord3_geo_radians
    ];
  };

  function reload() {
    var phi1 = fnord3_geo_radians * parallels[0],
        phi2 = fnord3_geo_radians * parallels[1],
        lat0 = fnord3_geo_radians * origin[1],
        s = Math.sin(phi1),
        c = Math.cos(phi1);
    lng0 = fnord3_geo_radians * origin[0];
    n = .5 * (s + Math.sin(phi2));
    C = c * c + 2 * n * s;
    p0 = Math.sqrt(C - 2 * n * Math.sin(lat0)) / n;
    return albers;
  }

  albers.origin = function(x) {
    if (!arguments.length) return origin;
    origin = [+x[0], +x[1]];
    return reload();
  };

  albers.parallels = function(x) {
    if (!arguments.length) return parallels;
    parallels = [+x[0], +x[1]];
    return reload();
  };

  albers.scale = function(x) {
    if (!arguments.length) return scale;
    scale = +x;
    return albers;
  };

  albers.translate = function(x) {
    if (!arguments.length) return translate;
    translate = [+x[0], +x[1]];
    return albers;
  };

  return reload();
};

// A composite projection for the United States, 960x500. The set of standard
// parallels for each region comes from USGS, which is published here:
// http://egsc.usgs.gov/isb/pubs/MapProjections/projections.html#albers
// TODO allow the composite projection to be rescaled?
fnord3.geo.albersUsa = function() {
  var lower48 = fnord3.geo.albers();

  var alaska = fnord3.geo.albers()
      .origin([-160, 60])
      .parallels([55, 65]);

  var hawaii = fnord3.geo.albers()
      .origin([-160, 20])
      .parallels([8, 18]);

  var puertoRico = fnord3.geo.albers()
      .origin([-60, 10])
      .parallels([8, 18]);

  function albersUsa(coordinates) {
    var lon = coordinates[0],
        lat = coordinates[1];
    return (lat > 50 ? alaska
        : lon < -140 ? hawaii
        : lat < 21 ? puertoRico
        : lower48)(coordinates);
  }

  albersUsa.scale = function(x) {
    if (!arguments.length) return lower48.scale();
    lower48.scale(x);
    alaska.scale(x * .6);
    hawaii.scale(x);
    puertoRico.scale(x * 1.5);
    return albersUsa.translate(lower48.translate());
  };

  albersUsa.translate = function(x) {
    if (!arguments.length) return lower48.translate();
    var dz = lower48.scale() / 1000,
        dx = x[0],
        dy = x[1];
    lower48.translate(x);
    alaska.translate([dx - 400 * dz, dy + 170 * dz]);
    hawaii.translate([dx - 190 * dz, dy + 200 * dz]);
    puertoRico.translate([dx + 580 * dz, dy + 430 * dz]);
    return albersUsa;
  };

  return albersUsa.scale(lower48.scale());
};
fnord3.geo.bonne = function() {
  var scale = 200,
      translate = [480, 250],
      x0, // origin longitude in radians
      y0, // origin latitude in radians
      y1, // parallel latitude in radians
      c1; // cot(y1)

  function bonne(coordinates) {
    var x = coordinates[0] * fnord3_geo_radians - x0,
        y = coordinates[1] * fnord3_geo_radians - y0;
    if (y1) {
      var p = c1 + y1 - y, E = x * Math.cos(y) / p;
      x = p * Math.sin(E);
      y = p * Math.cos(E) - c1;
    } else {
      x *= Math.cos(y);
      y *= -1;
    }
    return [
      scale * x + translate[0],
      scale * y + translate[1]
    ];
  }

  bonne.invert = function(coordinates) {
    var x = (coordinates[0] - translate[0]) / scale,
        y = (coordinates[1] - translate[1]) / scale;
    if (y1) {
      var c = c1 + y, p = Math.sqrt(x * x + c * c);
      y = c1 + y1 - p;
      x = x0 + p * Math.atan2(x, c) / Math.cos(y);
    } else {
      y *= -1;
      x /= Math.cos(y);
    }
    return [
      x / fnord3_geo_radians,
      y / fnord3_geo_radians
    ];
  };

  // 90° for Werner, 0° for Sinusoidal
  bonne.parallel = function(x) {
    if (!arguments.length) return y1 / fnord3_geo_radians;
    c1 = 1 / Math.tan(y1 = x * fnord3_geo_radians);
    return bonne;
  };

  bonne.origin = function(x) {
    if (!arguments.length) return [x0 / fnord3_geo_radians, y0 / fnord3_geo_radians];
    x0 = x[0] * fnord3_geo_radians;
    y0 = x[1] * fnord3_geo_radians;
    return bonne;
  };

  bonne.scale = function(x) {
    if (!arguments.length) return scale;
    scale = +x;
    return bonne;
  };

  bonne.translate = function(x) {
    if (!arguments.length) return translate;
    translate = [+x[0], +x[1]];
    return bonne;
  };

  return bonne.origin([0, 0]).parallel(45);
};
fnord3.geo.equirectangular = function() {
  var scale = 500,
      translate = [480, 250];

  function equirectangular(coordinates) {
    var x = coordinates[0] / 360,
        y = -coordinates[1] / 360;
    return [
      scale * x + translate[0],
      scale * y + translate[1]
    ];
  }

  equirectangular.invert = function(coordinates) {
    var x = (coordinates[0] - translate[0]) / scale,
        y = (coordinates[1] - translate[1]) / scale;
    return [
      360 * x,
      -360 * y
    ];
  };

  equirectangular.scale = function(x) {
    if (!arguments.length) return scale;
    scale = +x;
    return equirectangular;
  };

  equirectangular.translate = function(x) {
    if (!arguments.length) return translate;
    translate = [+x[0], +x[1]];
    return equirectangular;
  };

  return equirectangular;
};
fnord3.geo.mercator = function() {
  var scale = 500,
      translate = [480, 250];

  function mercator(coordinates) {
    var x = coordinates[0] / 360,
        y = -(Math.log(Math.tan(Math.PI / 4 + coordinates[1] * fnord3_geo_radians / 2)) / fnord3_geo_radians) / 360;
    return [
      scale * x + translate[0],
      scale * Math.max(-.5, Math.min(.5, y)) + translate[1]
    ];
  }

  mercator.invert = function(coordinates) {
    var x = (coordinates[0] - translate[0]) / scale,
        y = (coordinates[1] - translate[1]) / scale;
    return [
      360 * x,
      2 * Math.atan(Math.exp(-360 * y * fnord3_geo_radians)) / fnord3_geo_radians - 90
    ];
  };

  mercator.scale = function(x) {
    if (!arguments.length) return scale;
    scale = +x;
    return mercator;
  };

  mercator.translate = function(x) {
    if (!arguments.length) return translate;
    translate = [+x[0], +x[1]];
    return mercator;
  };

  return mercator;
};
function fnord3_geo_type(types, defaultValue) {
  return function(object) {
    return object && types.hasOwnProperty(object.type) ? types[object.type](object) : defaultValue;
  };
}
/**
 * Returns a function that, given a GeoJSON object (e.g., a feature), returns
 * the corresponding SVG path. The function can be customized by overriding the
 * projection. Point features are mapped to circles with a default radius of
 * 4.5px; the radius can be specified either as a constant or a function that
 * is evaluated per object.
 */
fnord3.geo.path = function() {
  var pointRadius = 4.5,
      pointCircle = fnord3_path_circle(pointRadius),
      projection = fnord3.geo.albersUsa();

  function path(d, i) {
    if (typeof pointRadius === "function") {
      pointCircle = fnord3_path_circle(pointRadius.apply(this, arguments));
    }
    return pathType(d) || null;
  }

  function project(coordinates) {
    return projection(coordinates).join(",");
  }

  var pathType = fnord3_geo_type({

    FeatureCollection: function(o) {
      var path = [],
          features = o.features,
          i = -1, // features.index
          n = features.length;
      while (++i < n) path.push(pathType(features[i].geometry));
      return path.join("");
    },

    Feature: function(o) {
      return pathType(o.geometry);
    },

    Point: function(o) {
      return "M" + project(o.coordinates) + pointCircle;
    },

    MultiPoint: function(o) {
      var path = [],
          coordinates = o.coordinates,
          i = -1, // coordinates.index
          n = coordinates.length;
      while (++i < n) path.push("M", project(coordinates[i]), pointCircle);
      return path.join("");
    },

    LineString: function(o) {
      var path = ["M"],
          coordinates = o.coordinates,
          i = -1, // coordinates.index
          n = coordinates.length;
      while (++i < n) path.push(project(coordinates[i]), "L");
      path.pop();
      return path.join("");
    },

    MultiLineString: function(o) {
      var path = [],
          coordinates = o.coordinates,
          i = -1, // coordinates.index
          n = coordinates.length,
          subcoordinates, // coordinates[i]
          j, // subcoordinates.index
          m; // subcoordinates.length
      while (++i < n) {
        subcoordinates = coordinates[i];
        j = -1;
        m = subcoordinates.length;
        path.push("M");
        while (++j < m) path.push(project(subcoordinates[j]), "L");
        path.pop();
      }
      return path.join("");
    },

    Polygon: function(o) {
      var path = [],
          coordinates = o.coordinates,
          i = -1, // coordinates.index
          n = coordinates.length,
          subcoordinates, // coordinates[i]
          j, // subcoordinates.index
          m; // subcoordinates.length
      while (++i < n) {
        subcoordinates = coordinates[i];
        j = -1;
        if ((m = subcoordinates.length - 1) > 0) {
          path.push("M");
          while (++j < m) path.push(project(subcoordinates[j]), "L");
          path[path.length - 1] = "Z";
        }
      }
      return path.join("");
    },

    MultiPolygon: function(o) {
      var path = [],
          coordinates = o.coordinates,
          i = -1, // coordinates index
          n = coordinates.length,
          subcoordinates, // coordinates[i]
          j, // subcoordinates index
          m, // subcoordinates.length
          subsubcoordinates, // subcoordinates[j]
          k, // subsubcoordinates index
          p; // subsubcoordinates.length
      while (++i < n) {
        subcoordinates = coordinates[i];
        j = -1;
        m = subcoordinates.length;
        while (++j < m) {
          subsubcoordinates = subcoordinates[j];
          k = -1;
          if ((p = subsubcoordinates.length - 1) > 0) {
            path.push("M");
            while (++k < p) path.push(project(subsubcoordinates[k]), "L");
            path[path.length - 1] = "Z";
          }
        }
      }
      return path.join("");
    },

    GeometryCollection: function(o) {
      var path = [],
          geometries = o.geometries,
          i = -1, // geometries index
          n = geometries.length;
      while (++i < n) path.push(pathType(geometries[i]));
      return path.join("");
    }

  });

  var areaType = path.area = fnord3_geo_type({

    FeatureCollection: function(o) {
      var area = 0,
          features = o.features,
          i = -1, // features.index
          n = features.length;
      while (++i < n) area += areaType(features[i]);
      return area;
    },

    Feature: function(o) {
      return areaType(o.geometry);
    },

    Polygon: function(o) {
      return polygonArea(o.coordinates);
    },

    MultiPolygon: function(o) {
      var sum = 0,
          coordinates = o.coordinates,
          i = -1, // coordinates index
          n = coordinates.length;
      while (++i < n) sum += polygonArea(coordinates[i]);
      return sum;
    },

    GeometryCollection: function(o) {
      var sum = 0,
          geometries = o.geometries,
          i = -1, // geometries index
          n = geometries.length;
      while (++i < n) sum += areaType(geometries[i]);
      return sum;
    }

  }, 0);

  function polygonArea(coordinates) {
    var sum = area(coordinates[0]), // exterior ring
        i = 0, // coordinates.index
        n = coordinates.length;
    while (++i < n) sum -= area(coordinates[i]); // holes
    return sum;
  }

  function polygonCentroid(coordinates) {
    var polygon = fnord3.geom.polygon(coordinates[0].map(projection)), // exterior ring
        area = polygon.area(),
        centroid = polygon.centroid(area < 0 ? (area *= -1, 1) : -1),
        x = centroid[0],
        y = centroid[1],
        z = area,
        i = 0, // coordinates index
        n = coordinates.length;
    while (++i < n) {
      polygon = fnord3.geom.polygon(coordinates[i].map(projection)); // holes
      area = polygon.area();
      centroid = polygon.centroid(area < 0 ? (area *= -1, 1) : -1);
      x -= centroid[0];
      y -= centroid[1];
      z -= area;
    }
    return [x, y, 6 * z]; // weighted centroid
  }

  var centroidType = path.centroid = fnord3_geo_type({

    // TODO FeatureCollection
    // TODO Point
    // TODO MultiPoint
    // TODO LineString
    // TODO MultiLineString
    // TODO GeometryCollection

    Feature: function(o) {
      return centroidType(o.geometry);
    },

    Polygon: function(o) {
      var centroid = polygonCentroid(o.coordinates);
      return [centroid[0] / centroid[2], centroid[1] / centroid[2]];
    },

    MultiPolygon: function(o) {
      var area = 0,
          coordinates = o.coordinates,
          centroid,
          x = 0,
          y = 0,
          z = 0,
          i = -1, // coordinates index
          n = coordinates.length;
      while (++i < n) {
        centroid = polygonCentroid(coordinates[i]);
        x += centroid[0];
        y += centroid[1];
        z += centroid[2];
      }
      return [x / z, y / z];
    }

  });

  function area(coordinates) {
    return Math.abs(fnord3.geom.polygon(coordinates.map(projection)).area());
  }

  path.projection = function(x) {
    projection = x;
    return path;
  };

  path.pointRadius = function(x) {
    if (typeof x === "function") pointRadius = x;
    else {
      pointRadius = +x;
      pointCircle = fnord3_path_circle(pointRadius);
    }
    return path;
  };

  return path;
};

function fnord3_path_circle(radius) {
  return "m0," + radius
      + "a" + radius + "," + radius + " 0 1,1 0," + (-2 * radius)
      + "a" + radius + "," + radius + " 0 1,1 0," + (+2 * radius)
      + "z";
}
/**
 * Given a GeoJSON object, returns the corresponding bounding box. The bounding
 * box is represented by a two-dimensional array: [[left, bottom], [right,
 * top]], where left is the minimum longitude, bottom is the minimum latitude,
 * right is maximum longitude, and top is the maximum latitude.
 */
fnord3.geo.bounds = function(feature) {
  var left = Infinity,
      bottom = Infinity,
      right = -Infinity,
      top = -Infinity;
  fnord3_geo_bounds(feature, function(x, y) {
    if (x < left) left = x;
    if (x > right) right = x;
    if (y < bottom) bottom = y;
    if (y > top) top = y;
  });
  return [[left, bottom], [right, top]];
};

function fnord3_geo_bounds(o, f) {
  if (fnord3_geo_boundsTypes.hasOwnProperty(o.type)) fnord3_geo_boundsTypes[o.type](o, f);
}

var fnord3_geo_boundsTypes = {
  Feature: fnord3_geo_boundsFeature,
  FeatureCollection: fnord3_geo_boundsFeatureCollection,
  GeometryCollection: fnord3_geo_boundsGeometryCollection,
  LineString: fnord3_geo_boundsLineString,
  MultiLineString: fnord3_geo_boundsMultiLineString,
  MultiPoint: fnord3_geo_boundsLineString,
  MultiPolygon: fnord3_geo_boundsMultiPolygon,
  Point: fnord3_geo_boundsPoint,
  Polygon: fnord3_geo_boundsPolygon
};

function fnord3_geo_boundsFeature(o, f) {
  fnord3_geo_bounds(o.geometry, f);
}

function fnord3_geo_boundsFeatureCollection(o, f) {
  for (var a = o.features, i = 0, n = a.length; i < n; i++) {
    fnord3_geo_bounds(a[i].geometry, f);
  }
}

function fnord3_geo_boundsGeometryCollection(o, f) {
  for (var a = o.geometries, i = 0, n = a.length; i < n; i++) {
    fnord3_geo_bounds(a[i], f);
  }
}

function fnord3_geo_boundsLineString(o, f) {
  for (var a = o.coordinates, i = 0, n = a.length; i < n; i++) {
    f.apply(null, a[i]);
  }
}

function fnord3_geo_boundsMultiLineString(o, f) {
  for (var a = o.coordinates, i = 0, n = a.length; i < n; i++) {
    for (var b = a[i], j = 0, m = b.length; j < m; j++) {
      f.apply(null, b[j]);
    }
  }
}

function fnord3_geo_boundsMultiPolygon(o, f) {
  for (var a = o.coordinates, i = 0, n = a.length; i < n; i++) {
    for (var b = a[i][0], j = 0, m = b.length; j < m; j++) {
      f.apply(null, b[j]);
    }
  }
}

function fnord3_geo_boundsPoint(o, f) {
  f.apply(null, o.coordinates);
}

function fnord3_geo_boundsPolygon(o, f) {
  for (var a = o.coordinates[0], i = 0, n = a.length; i < n; i++) {
    f.apply(null, a[i]);
  }
}
// TODO breakAtDateLine?

fnord3.geo.circle = function() {
  var origin = [0, 0],
      degrees = 90 - 1e-2,
      radians = degrees * fnord3_geo_radians,
      arc = fnord3.geo.greatArc().target(fnord3_identity);

  function circle() {
    // TODO render a circle as a Polygon
  }

  function visible(point) {
    return arc.distance(point) < radians;
  }

  circle.clip = function(d) {
    arc.source(typeof origin === "function" ? origin.apply(this, arguments) : origin);
    return clipType(d);
  };

  var clipType = fnord3_geo_type({

    FeatureCollection: function(o) {
      var features = o.features.map(clipType).filter(fnord3_identity);
      return features && (o = Object.create(o), o.features = features, o);
    },

    Feature: function(o) {
      var geometry = clipType(o.geometry);
      return geometry && (o = Object.create(o), o.geometry = geometry, o);
    },

    Point: function(o) {
      return visible(o.coordinates) && o;
    },

    MultiPoint: function(o) {
      var coordinates = o.coordinates.filter(visible);
      return coordinates.length && {
        type: o.type,
        coordinates: coordinates
      };
    },

    LineString: function(o) {
      var coordinates = clip(o.coordinates);
      return coordinates.length && (o = Object.create(o), o.coordinates = coordinates, o);
    },

    MultiLineString: function(o) {
      var coordinates = o.coordinates.map(clip).filter(function(d) { return d.length; });
      return coordinates.length && (o = Object.create(o), o.coordinates = coordinates, o);
    },

    Polygon: function(o) {
      var coordinates = o.coordinates.map(clip);
      return coordinates[0].length && (o = Object.create(o), o.coordinates = coordinates, o);
    },

    MultiPolygon: function(o) {
      var coordinates = o.coordinates.map(function(d) { return d.map(clip); }).filter(function(d) { return d[0].length; });
      return coordinates.length && (o = Object.create(o), o.coordinates = coordinates, o);
    },

    GeometryCollection: function(o) {
      var geometries = o.geometries.map(clipType).filter(fnord3_identity);
      return geometries.length && (o = Object.create(o), o.geometries = geometries, o);
    }

  });

  function clip(coordinates) {
    var i = -1,
        n = coordinates.length,
        clipped = [],
        p0,
        p1,
        p2,
        d0,
        d1;

    while (++i < n) {
      d1 = arc.distance(p2 = coordinates[i]);
      if (d1 < radians) {
        if (p1) clipped.push(fnord3_geo_greatArcInterpolate(p1, p2)((d0 - radians) / (d0 - d1)));
        clipped.push(p2);
        p0 = p1 = null;
      } else {
        p1 = p2;
        if (!p0 && clipped.length) {
          clipped.push(fnord3_geo_greatArcInterpolate(clipped[clipped.length - 1], p1)((radians - d0) / (d1 - d0)));
          p0 = p1;
        }
      }
      d0 = d1;
    }

    if (p1 && clipped.length) {
      d1 = arc.distance(p2 = clipped[0]);
      clipped.push(fnord3_geo_greatArcInterpolate(p1, p2)((d0 - radians) / (d0 - d1)));
    }

    return resample(clipped);
  }

  // Resample coordinates, creating great arcs between each.
  function resample(coordinates) {
    var i = 0,
        n = coordinates.length,
        j,
        m,
        resampled = n ? [coordinates[0]] : coordinates,
        resamples,
        origin = arc.source();

    while (++i < n) {
      resamples = arc.source(coordinates[i - 1])(coordinates[i]).coordinates;
      for (j = 0, m = resamples.length; ++j < m;) resampled.push(resamples[j]);
    }

    arc.source(origin);
    return resampled;
  }

  circle.origin = function(x) {
    if (!arguments.length) return origin;
    origin = x;
    return circle;
  };

  circle.angle = function(x) {
    if (!arguments.length) return degrees;
    radians = (degrees = +x) * fnord3_geo_radians;
    return circle;
  };

  // Precision is specified in degrees.
  circle.precision = function(x) {
    if (!arguments.length) return arc.precision();
    arc.precision(x);
    return circle;
  };

  return circle;
}
fnord3.geo.greatArc = function() {
  var source = fnord3_geo_greatArcSource,
      target = fnord3_geo_greatArcTarget,
      precision = 6 * fnord3_geo_radians;

  function greatArc() {
    var a = typeof source === "function" ? source.apply(this, arguments) : source,
        b = typeof target === "function" ? target.apply(this, arguments) : target,
        i = fnord3_geo_greatArcInterpolate(a, b),
        dt = precision / i.d,
        t = 0,
        coordinates = [a];
    while ((t += dt) < 1) coordinates.push(i(t));
    coordinates.push(b);
    return {
      type: "LineString",
      coordinates: coordinates
    };
  }

  // Length returned in radians; multiply by radius for distance.
  greatArc.distance = function() {
    var a = typeof source === "function" ? source.apply(this, arguments) : source,
        b = typeof target === "function" ? target.apply(this, arguments) : target;
     return fnord3_geo_greatArcInterpolate(a, b).d;
  };

  greatArc.source = function(x) {
    if (!arguments.length) return source;
    source = x;
    return greatArc;
  };

  greatArc.target = function(x) {
    if (!arguments.length) return target;
    target = x;
    return greatArc;
  };

  // Precision is specified in degrees.
  greatArc.precision = function(x) {
    if (!arguments.length) return precision / fnord3_geo_radians;
    precision = x * fnord3_geo_radians;
    return greatArc;
  };

  return greatArc;
};

function fnord3_geo_greatArcSource(d) {
  return d.source;
}

function fnord3_geo_greatArcTarget(d) {
  return d.target;
}

function fnord3_geo_greatArcInterpolate(a, b) {
  var x0 = a[0] * fnord3_geo_radians, cx0 = Math.cos(x0), sx0 = Math.sin(x0),
      y0 = a[1] * fnord3_geo_radians, cy0 = Math.cos(y0), sy0 = Math.sin(y0),
      x1 = b[0] * fnord3_geo_radians, cx1 = Math.cos(x1), sx1 = Math.sin(x1),
      y1 = b[1] * fnord3_geo_radians, cy1 = Math.cos(y1), sy1 = Math.sin(y1),
      d = interpolate.d = Math.acos(Math.max(-1, Math.min(1, sy0 * sy1 + cy0 * cy1 * Math.cos(x1 - x0)))),
      sd = Math.sin(d);

  // From http://williams.best.vwh.net/avform.htm#Intermediate
  function interpolate(t) {
    var A = Math.sin(d - (t *= d)) / sd,
        B = Math.sin(t) / sd,
        x = A * cy0 * cx0 + B * cy1 * cx1,
        y = A * cy0 * sx0 + B * cy1 * sx1,
        z = A * sy0       + B * sy1;
    return [
      Math.atan2(y, x) / fnord3_geo_radians,
      Math.atan2(z, Math.sqrt(x * x + y * y)) / fnord3_geo_radians
    ];
  }

  return interpolate;
}
fnord3.geo.greatCircle = fnord3.geo.circle;
fnord3.geom = {};
/**
 * Computes a contour for a given input grid function using the <a
 * href="http://en.wikipedia.org/wiki/Marching_squares">marching
 * squares</a> algorithm. Returns the contour polygon as an array of points.
 *
 * @param grid a two-input function(x, y) that returns true for values
 * inside the contour and false for values outside the contour.
 * @param start an optional starting point [x, y] on the grid.
 * @returns polygon [[x1, y1], [x2, y2], …]
 */
fnord3.geom.contour = function(grid, start) {
  var s = start || fnord3_geom_contourStart(grid), // starting point
      c = [],    // contour polygon
      x = s[0],  // current x position
      y = s[1],  // current y position
      dx = 0,    // next x direction
      dy = 0,    // next y direction
      pdx = NaN, // previous x direction
      pdy = NaN, // previous y direction
      i = 0;

  do {
    // determine marching squares index
    i = 0;
    if (grid(x-1, y-1)) i += 1;
    if (grid(x,   y-1)) i += 2;
    if (grid(x-1, y  )) i += 4;
    if (grid(x,   y  )) i += 8;

    // determine next direction
    if (i === 6) {
      dx = pdy === -1 ? -1 : 1;
      dy = 0;
    } else if (i === 9) {
      dx = 0;
      dy = pdx === 1 ? -1 : 1;
    } else {
      dx = fnord3_geom_contourDx[i];
      dy = fnord3_geom_contourDy[i];
    }

    // update contour polygon
    if (dx != pdx && dy != pdy) {
      c.push([x, y]);
      pdx = dx;
      pdy = dy;
    }

    x += dx;
    y += dy;
  } while (s[0] != x || s[1] != y);

  return c;
};

// lookup tables for marching directions
var fnord3_geom_contourDx = [1, 0, 1, 1,-1, 0,-1, 1,0, 0,0,0,-1, 0,-1,NaN],
    fnord3_geom_contourDy = [0,-1, 0, 0, 0,-1, 0, 0,1,-1,1,1, 0,-1, 0,NaN];

function fnord3_geom_contourStart(grid) {
  var x = 0,
      y = 0;

  // search for a starting point; begin at origin
  // and proceed along outward-expanding diagonals
  while (true) {
    if (grid(x,y)) {
      return [x,y];
    }
    if (x === 0) {
      x = y + 1;
      y = 0;
    } else {
      x = x - 1;
      y = y + 1;
    }
  }
}
/**
 * Computes the 2D convex hull of a set of points using Graham's scanning
 * algorithm. The algorithm has been implemented as described in Cormen,
 * Leiserson, and Rivest's Introduction to Algorithms. The running time of
 * this algorithm is O(n log n), where n is the number of input points.
 *
 * @param vertices [[x1, y1], [x2, y2], …]
 * @returns polygon [[x1, y1], [x2, y2], …]
 */
fnord3.geom.hull = function(vertices) {
  if (vertices.length < 3) return [];

  var len = vertices.length,
      plen = len - 1,
      points = [],
      stack = [],
      i, j, h = 0, x1, y1, x2, y2, u, v, a, sp;

  // find the starting ref point: leftmost point with the minimum y coord
  for (i=1; i<len; ++i) {
    if (vertices[i][1] < vertices[h][1]) {
      h = i;
    } else if (vertices[i][1] == vertices[h][1]) {
      h = (vertices[i][0] < vertices[h][0] ? i : h);
    }
  }

  // calculate polar angles from ref point and sort
  for (i=0; i<len; ++i) {
    if (i === h) continue;
    y1 = vertices[i][1] - vertices[h][1];
    x1 = vertices[i][0] - vertices[h][0];
    points.push({angle: Math.atan2(y1, x1), index: i});
  }
  points.sort(function(a, b) { return a.angle - b.angle; });

  // toss out duplicate angles
  a = points[0].angle;
  v = points[0].index;
  u = 0;
  for (i=1; i<plen; ++i) {
    j = points[i].index;
    if (a == points[i].angle) {
      // keep angle for point most distant from the reference
      x1 = vertices[v][0] - vertices[h][0];
      y1 = vertices[v][1] - vertices[h][1];
      x2 = vertices[j][0] - vertices[h][0];
      y2 = vertices[j][1] - vertices[h][1];
      if ((x1*x1 + y1*y1) >= (x2*x2 + y2*y2)) {
        points[i].index = -1;
      } else {
        points[u].index = -1;
        a = points[i].angle;
        u = i;
        v = j;
      }
    } else {
      a = points[i].angle;
      u = i;
      v = j;
    }
  }

  // initialize the stack
  stack.push(h);
  for (i=0, j=0; i<2; ++j) {
    if (points[j].index !== -1) {
      stack.push(points[j].index);
      i++;
    }
  }
  sp = stack.length;

  // do graham's scan
  for (; j<plen; ++j) {
    if (points[j].index === -1) continue; // skip tossed out points
    while (!fnord3_geom_hullCCW(stack[sp-2], stack[sp-1], points[j].index, vertices)) {
      --sp;
    }
    stack[sp++] = points[j].index;
  }

  // construct the hull
  var poly = [];
  for (i=0; i<sp; ++i) {
    poly.push(vertices[stack[i]]);
  }
  return poly;
}

// are three points in counter-clockwise order?
function fnord3_geom_hullCCW(i1, i2, i3, v) {
  var t, a, b, c, d, e, f;
  t = v[i1]; a = t[0]; b = t[1];
  t = v[i2]; c = t[0]; d = t[1];
  t = v[i3]; e = t[0]; f = t[1];
  return ((f-b)*(c-a) - (d-b)*(e-a)) > 0;
}
// Note: requires coordinates to be counterclockwise and convex!
fnord3.geom.polygon = function(coordinates) {

  coordinates.area = function() {
    var i = 0,
        n = coordinates.length,
        a = coordinates[n - 1][0] * coordinates[0][1],
        b = coordinates[n - 1][1] * coordinates[0][0];
    while (++i < n) {
      a += coordinates[i - 1][0] * coordinates[i][1];
      b += coordinates[i - 1][1] * coordinates[i][0];
    }
    return (b - a) * .5;
  };

  coordinates.centroid = function(k) {
    var i = -1,
        n = coordinates.length,
        x = 0,
        y = 0,
        a,
        b = coordinates[n - 1],
        c;
    if (!arguments.length) k = -1 / (6 * coordinates.area());
    while (++i < n) {
      a = b;
      b = coordinates[i];
      c = a[0] * b[1] - b[0] * a[1];
      x += (a[0] + b[0]) * c;
      y += (a[1] + b[1]) * c;
    }
    return [x * k, y * k];
  };

  // The Sutherland-Hodgman clipping algorithm.
  coordinates.clip = function(subject) {
    var input,
        i = -1,
        n = coordinates.length,
        j,
        m,
        a = coordinates[n - 1],
        b,
        c,
        d;
    while (++i < n) {
      input = subject.slice();
      subject.length = 0;
      b = coordinates[i];
      c = input[(m = input.length) - 1];
      j = -1;
      while (++j < m) {
        d = input[j];
        if (fnord3_geom_polygonInside(d, a, b)) {
          if (!fnord3_geom_polygonInside(c, a, b)) {
            subject.push(fnord3_geom_polygonIntersect(c, d, a, b));
          }
          subject.push(d);
        } else if (fnord3_geom_polygonInside(c, a, b)) {
          subject.push(fnord3_geom_polygonIntersect(c, d, a, b));
        }
        c = d;
      }
      a = b;
    }
    return subject;
  };

  return coordinates;
};

function fnord3_geom_polygonInside(p, a, b) {
  return (b[0] - a[0]) * (p[1] - a[1]) < (b[1] - a[1]) * (p[0] - a[0]);
}

// Intersect two infinite lines cd and ab.
function fnord3_geom_polygonIntersect(c, d, a, b) {
  var x1 = c[0], x2 = d[0], x3 = a[0], x4 = b[0],
      y1 = c[1], y2 = d[1], y3 = a[1], y4 = b[1],
      x13 = x1 - x3,
      x21 = x2 - x1,
      x43 = x4 - x3,
      y13 = y1 - y3,
      y21 = y2 - y1,
      y43 = y4 - y3,
      ua = (x43 * y13 - y43 * x13) / (y43 * x21 - x43 * y21);
  return [x1 + ua * x21, y1 + ua * y21];
}
// Adapted from Nicolas Garcia Belmonte's JIT implementation:
// http://blog.thejit.org/2010/02/12/voronoi-tessellation/
// http://blog.thejit.org/assets/voronoijs/voronoi.js
// See lib/jit/LICENSE for details.

// Notes:
//
// This implementation does not clip the returned polygons, so if you want to
// clip them to a particular shape you will need to do that either in SVG or by
// post-processing with fnord3.geom.polygon's clip method.
//
// If any vertices are coincident or have NaN positions, the behavior of this
// method is undefined. Most likely invalid polygons will be returned. You
// should filter invalid points, and consolidate coincident points, before
// computing the tessellation.

/**
 * @param vertices [[x1, y1], [x2, y2], …]
 * @returns polygons [[[x1, y1], [x2, y2], …], …]
 */
fnord3.geom.voronoi = function(vertices) {
  var polygons = vertices.map(function() { return []; });

  fnord3_voronoi_tessellate(vertices, function(e) {
    var s1,
        s2,
        x1,
        x2,
        y1,
        y2;
    if (e.a === 1 && e.b >= 0) {
      s1 = e.ep.r;
      s2 = e.ep.l;
    } else {
      s1 = e.ep.l;
      s2 = e.ep.r;
    }
    if (e.a === 1) {
      y1 = s1 ? s1.y : -1e6;
      x1 = e.c - e.b * y1;
      y2 = s2 ? s2.y : 1e6;
      x2 = e.c - e.b * y2;
    } else {
      x1 = s1 ? s1.x : -1e6;
      y1 = e.c - e.a * x1;
      x2 = s2 ? s2.x : 1e6;
      y2 = e.c - e.a * x2;
    }
    var v1 = [x1, y1],
        v2 = [x2, y2];
    polygons[e.region.l.index].push(v1, v2);
    polygons[e.region.r.index].push(v1, v2);
  });

  // Reconnect the polygon segments into counterclockwise loops.
  return polygons.map(function(polygon, i) {
    var cx = vertices[i][0],
        cy = vertices[i][1];
    polygon.forEach(function(v) {
      v.angle = Math.atan2(v[0] - cx, v[1] - cy);
    });
    return polygon.sort(function(a, b) {
      return a.angle - b.angle;
    }).filter(function(d, i) {
      return !i || (d.angle - polygon[i - 1].angle > 1e-10);
    });
  });
};

var fnord3_voronoi_opposite = {"l": "r", "r": "l"};

function fnord3_voronoi_tessellate(vertices, callback) {

  var Sites = {
    list: vertices
      .map(function(v, i) {
        return {
          index: i,
          x: v[0],
          y: v[1]
        };
      })
      .sort(function(a, b) {
        return a.y < b.y ? -1
          : a.y > b.y ? 1
          : a.x < b.x ? -1
          : a.x > b.x ? 1
          : 0;
      }),
    bottomSite: null
  };

  var EdgeList = {
    list: [],
    leftEnd: null,
    rightEnd: null,

    init: function() {
      EdgeList.leftEnd = EdgeList.createHalfEdge(null, "l");
      EdgeList.rightEnd = EdgeList.createHalfEdge(null, "l");
      EdgeList.leftEnd.r = EdgeList.rightEnd;
      EdgeList.rightEnd.l = EdgeList.leftEnd;
      EdgeList.list.unshift(EdgeList.leftEnd, EdgeList.rightEnd);
    },

    createHalfEdge: function(edge, side) {
      return {
        edge: edge,
        side: side,
        vertex: null,
        "l": null,
        "r": null
      };
    },

    insert: function(lb, he) {
      he.l = lb;
      he.r = lb.r;
      lb.r.l = he;
      lb.r = he;
    },

    leftBound: function(p) {
      var he = EdgeList.leftEnd;
      do {
        he = he.r;
      } while (he != EdgeList.rightEnd && Geom.rightOf(he, p));
      he = he.l;
      return he;
    },

    del: function(he) {
      he.l.r = he.r;
      he.r.l = he.l;
      he.edge = null;
    },

    right: function(he) {
      return he.r;
    },

    left: function(he) {
      return he.l;
    },

    leftRegion: function(he) {
      return he.edge == null
          ? Sites.bottomSite
          : he.edge.region[he.side];
    },

    rightRegion: function(he) {
      return he.edge == null
          ? Sites.bottomSite
          : he.edge.region[fnord3_voronoi_opposite[he.side]];
    }
  };

  var Geom = {

    bisect: function(s1, s2) {
      var newEdge = {
        region: {"l": s1, "r": s2},
        ep: {"l": null, "r": null}
      };

      var dx = s2.x - s1.x,
          dy = s2.y - s1.y,
          adx = dx > 0 ? dx : -dx,
          ady = dy > 0 ? dy : -dy;

      newEdge.c = s1.x * dx + s1.y * dy
          + (dx * dx + dy * dy) * .5;

      if (adx > ady) {
        newEdge.a = 1;
        newEdge.b = dy / dx;
        newEdge.c /= dx;
      } else {
        newEdge.b = 1;
        newEdge.a = dx / dy;
        newEdge.c /= dy;
      }

      return newEdge;
    },

    intersect: function(el1, el2) {
      var e1 = el1.edge,
          e2 = el2.edge;
      if (!e1 || !e2 || (e1.region.r == e2.region.r)) {
        return null;
      }
      var d = (e1.a * e2.b) - (e1.b * e2.a);
      if (Math.abs(d) < 1e-10) {
        return null;
      }
      var xint = (e1.c * e2.b - e2.c * e1.b) / d,
          yint = (e2.c * e1.a - e1.c * e2.a) / d,
          e1r = e1.region.r,
          e2r = e2.region.r,
          el,
          e;
      if ((e1r.y < e2r.y) ||
         (e1r.y == e2r.y && e1r.x < e2r.x)) {
        el = el1;
        e = e1;
      } else {
        el = el2;
        e = e2;
      }
      var rightOfSite = (xint >= e.region.r.x);
      if ((rightOfSite && (el.side === "l")) ||
        (!rightOfSite && (el.side === "r"))) {
        return null;
      }
      return {
        x: xint,
        y: yint
      };
    },

    rightOf: function(he, p) {
      var e = he.edge,
          topsite = e.region.r,
          rightOfSite = (p.x > topsite.x);

      if (rightOfSite && (he.side === "l")) {
        return 1;
      }
      if (!rightOfSite && (he.side === "r")) {
        return 0;
      }
      if (e.a === 1) {
        var dyp = p.y - topsite.y,
            dxp = p.x - topsite.x,
            fast = 0,
            above = 0;

        if ((!rightOfSite && (e.b < 0)) ||
          (rightOfSite && (e.b >= 0))) {
          above = fast = (dyp >= e.b * dxp);
        } else {
          above = ((p.x + p.y * e.b) > e.c);
          if (e.b < 0) {
            above = !above;
          }
          if (!above) {
            fast = 1;
          }
        }
        if (!fast) {
          var dxs = topsite.x - e.region.l.x;
          above = (e.b * (dxp * dxp - dyp * dyp)) <
            (dxs * dyp * (1 + 2 * dxp / dxs + e.b * e.b));

          if (e.b < 0) {
            above = !above;
          }
        }
      } else /* e.b == 1 */ {
        var yl = e.c - e.a * p.x,
            t1 = p.y - yl,
            t2 = p.x - topsite.x,
            t3 = yl - topsite.y;

        above = (t1 * t1) > (t2 * t2 + t3 * t3);
      }
      return he.side === "l" ? above : !above;
    },

    endPoint: function(edge, side, site) {
      edge.ep[side] = site;
      if (!edge.ep[fnord3_voronoi_opposite[side]]) return;
      callback(edge);
    },

    distance: function(s, t) {
      var dx = s.x - t.x,
          dy = s.y - t.y;
      return Math.sqrt(dx * dx + dy * dy);
    }
  };

  var EventQueue = {
    list: [],

    insert: function(he, site, offset) {
      he.vertex = site;
      he.ystar = site.y + offset;
      for (var i=0, list=EventQueue.list, l=list.length; i<l; i++) {
        var next = list[i];
        if (he.ystar > next.ystar ||
          (he.ystar == next.ystar &&
          site.x > next.vertex.x)) {
          continue;
        } else {
          break;
        }
      }
      list.splice(i, 0, he);
    },

    del: function(he) {
      for (var i=0, ls=EventQueue.list, l=ls.length; i<l && (ls[i] != he); ++i) {}
      ls.splice(i, 1);
    },

    empty: function() { return EventQueue.list.length === 0; },

    nextEvent: function(he) {
      for (var i=0, ls=EventQueue.list, l=ls.length; i<l; ++i) {
        if (ls[i] == he) return ls[i+1];
      }
      return null;
    },

    min: function() {
      var elem = EventQueue.list[0];
      return {
        x: elem.vertex.x,
        y: elem.ystar
      };
    },

    extractMin: function() {
      return EventQueue.list.shift();
    }
  };

  EdgeList.init();
  Sites.bottomSite = Sites.list.shift();

  var newSite = Sites.list.shift(), newIntStar;
  var lbnd, rbnd, llbnd, rrbnd, bisector;
  var bot, top, temp, p, v;
  var e, pm;

  while (true) {
    if (!EventQueue.empty()) {
      newIntStar = EventQueue.min();
    }
    if (newSite && (EventQueue.empty()
      || newSite.y < newIntStar.y
      || (newSite.y == newIntStar.y
      && newSite.x < newIntStar.x))) { //new site is smallest
      lbnd = EdgeList.leftBound(newSite);
      rbnd = EdgeList.right(lbnd);
      bot = EdgeList.rightRegion(lbnd);
      e = Geom.bisect(bot, newSite);
      bisector = EdgeList.createHalfEdge(e, "l");
      EdgeList.insert(lbnd, bisector);
      p = Geom.intersect(lbnd, bisector);
      if (p) {
        EventQueue.del(lbnd);
        EventQueue.insert(lbnd, p, Geom.distance(p, newSite));
      }
      lbnd = bisector;
      bisector = EdgeList.createHalfEdge(e, "r");
      EdgeList.insert(lbnd, bisector);
      p = Geom.intersect(bisector, rbnd);
      if (p) {
        EventQueue.insert(bisector, p, Geom.distance(p, newSite));
      }
      newSite = Sites.list.shift();
    } else if (!EventQueue.empty()) { //intersection is smallest
      lbnd = EventQueue.extractMin();
      llbnd = EdgeList.left(lbnd);
      rbnd = EdgeList.right(lbnd);
      rrbnd = EdgeList.right(rbnd);
      bot = EdgeList.leftRegion(lbnd);
      top = EdgeList.rightRegion(rbnd);
      v = lbnd.vertex;
      Geom.endPoint(lbnd.edge, lbnd.side, v);
      Geom.endPoint(rbnd.edge, rbnd.side, v);
      EdgeList.del(lbnd);
      EventQueue.del(rbnd);
      EdgeList.del(rbnd);
      pm = "l";
      if (bot.y > top.y) {
        temp = bot;
        bot = top;
        top = temp;
        pm = "r";
      }
      e = Geom.bisect(bot, top);
      bisector = EdgeList.createHalfEdge(e, pm);
      EdgeList.insert(llbnd, bisector);
      Geom.endPoint(e, fnord3_voronoi_opposite[pm], v);
      p = Geom.intersect(llbnd, bisector);
      if (p) {
        EventQueue.del(llbnd);
        EventQueue.insert(llbnd, p, Geom.distance(p, bot));
      }
      p = Geom.intersect(bisector, rrbnd);
      if (p) {
        EventQueue.insert(bisector, p, Geom.distance(p, bot));
      }
    } else {
      break;
    }
  }//end while

  for (lbnd = EdgeList.right(EdgeList.leftEnd);
      lbnd != EdgeList.rightEnd;
      lbnd = EdgeList.right(lbnd)) {
    callback(lbnd.edge);
  }
}
/**
* @param vertices [[x1, y1], [x2, y2], …]
* @returns triangles [[[x1, y1], [x2, y2], [x3, y3]], …]
 */
fnord3.geom.delaunay = function(vertices) {
  var edges = vertices.map(function() { return []; }),
      triangles = [];

  // Use the Voronoi tessellation to determine Delaunay edges.
  fnord3_voronoi_tessellate(vertices, function(e) {
    edges[e.region.l.index].push(vertices[e.region.r.index]);
  });

  // Reconnect the edges into counterclockwise triangles.
  edges.forEach(function(edge, i) {
    var v = vertices[i],
        cx = v[0],
        cy = v[1];
    edge.forEach(function(v) {
      v.angle = Math.atan2(v[0] - cx, v[1] - cy);
    });
    edge.sort(function(a, b) {
      return a.angle - b.angle;
    });
    for (var j = 0, m = edge.length - 1; j < m; j++) {
      triangles.push([v, edge[j], edge[j + 1]]);
    }
  });

  return triangles;
};
// Constructs a new quadtree for the specified array of points. A quadtree is a
// two-dimensional recursive spatial subdivision. This implementation uses
// square partitions, dividing each square into four equally-sized squares. Each
// point exists in a unique node; if multiple points are in the same position,
// some points may be stored on internal nodes rather than leaf nodes. Quadtrees
// can be used to accelerate various spatial operations, such as the Barnes-Hut
// approximation for computing n-body forces, or collision detection.
fnord3.geom.quadtree = function(points, x1, y1, x2, y2) {
  var p,
      i = -1,
      n = points.length;

  // Type conversion for deprecated API.
  if (n && isNaN(points[0].x)) points = points.map(fnord3_geom_quadtreePoint);

  // Allow bounds to be specified explicitly.
  if (arguments.length < 5) {
    if (arguments.length === 3) {
      y2 = x2 = y1;
      y1 = x1;
    } else {
      x1 = y1 = Infinity;
      x2 = y2 = -Infinity;

      // Compute bounds.
      while (++i < n) {
        p = points[i];
        if (p.x < x1) x1 = p.x;
        if (p.y < y1) y1 = p.y;
        if (p.x > x2) x2 = p.x;
        if (p.y > y2) y2 = p.y;
      }

      // Squarify the bounds.
      var dx = x2 - x1,
          dy = y2 - y1;
      if (dx > dy) y2 = y1 + dx;
      else x2 = x1 + dy;
    }
  }

  // Recursively inserts the specified point p at the node n or one of its
  // descendants. The bounds are defined by [x1, x2] and [y1, y2].
  function insert(n, p, x1, y1, x2, y2) {
    if (isNaN(p.x) || isNaN(p.y)) return; // ignore invalid points
    if (n.leaf) {
      var v = n.point;
      if (v) {
        // If the point at this leaf node is at the same position as the new
        // point we are adding, we leave the point associated with the
        // internal node while adding the new point to a child node. This
        // avoids infinite recursion.
        if ((Math.abs(v.x - p.x) + Math.abs(v.y - p.y)) < .01) {
          insertChild(n, p, x1, y1, x2, y2);
        } else {
          n.point = null;
          insertChild(n, v, x1, y1, x2, y2);
          insertChild(n, p, x1, y1, x2, y2);
        }
      } else {
        n.point = p;
      }
    } else {
      insertChild(n, p, x1, y1, x2, y2);
    }
  }

  // Recursively inserts the specified point p into a descendant of node n. The
  // bounds are defined by [x1, x2] and [y1, y2].
  function insertChild(n, p, x1, y1, x2, y2) {
    // Compute the split point, and the quadrant in which to insert p.
    var sx = (x1 + x2) * .5,
        sy = (y1 + y2) * .5,
        right = p.x >= sx,
        bottom = p.y >= sy,
        i = (bottom << 1) + right;

    // Recursively insert into the child node.
    n.leaf = false;
    n = n.nodes[i] || (n.nodes[i] = fnord3_geom_quadtreeNode());

    // Update the bounds as we recurse.
    if (right) x1 = sx; else x2 = sx;
    if (bottom) y1 = sy; else y2 = sy;
    insert(n, p, x1, y1, x2, y2);
  }

  // Create the root node.
  var root = fnord3_geom_quadtreeNode();

  root.add = function(p) {
    insert(root, p, x1, y1, x2, y2);
  };

  root.visit = function(f) {
    fnord3_geom_quadtreeVisit(f, root, x1, y1, x2, y2);
  };

  // Insert all points.
  points.forEach(root.add);
  return root;
};

function fnord3_geom_quadtreeNode() {
  return {
    leaf: true,
    nodes: [],
    point: null
  };
}

function fnord3_geom_quadtreeVisit(f, node, x1, y1, x2, y2) {
  if (!f(node, x1, y1, x2, y2)) {
    var sx = (x1 + x2) * .5,
        sy = (y1 + y2) * .5,
        children = node.nodes;
    if (children[0]) fnord3_geom_quadtreeVisit(f, children[0], x1, y1, sx, sy);
    if (children[1]) fnord3_geom_quadtreeVisit(f, children[1], sx, y1, x2, sy);
    if (children[2]) fnord3_geom_quadtreeVisit(f, children[2], x1, sy, sx, y2);
    if (children[3]) fnord3_geom_quadtreeVisit(f, children[3], sx, sy, x2, y2);
  }
}

function fnord3_geom_quadtreePoint(p) {
  return {
    x: p[0],
    y: p[1]
  };
}
fnord3.time = {};

var fnord3_time = Date;

function fnord3_time_utc() {
  this._ = new Date(arguments.length > 1
      ? Date.UTC.apply(this, arguments)
      : arguments[0]);
}

fnord3_time_utc.prototype = {
  getDate: function() { return this._.getUTCDate(); },
  getDay: function() { return this._.getUTCDay(); },
  getFullYear: function() { return this._.getUTCFullYear(); },
  getHours: function() { return this._.getUTCHours(); },
  getMilliseconds: function() { return this._.getUTCMilliseconds(); },
  getMinutes: function() { return this._.getUTCMinutes(); },
  getMonth: function() { return this._.getUTCMonth(); },
  getSeconds: function() { return this._.getUTCSeconds(); },
  getTime: function() { return this._.getTime(); },
  getTimezoneOffset: function() { return 0; },
  valueOf: function() { return this._.valueOf(); },
  setDate: function() { fnord3_time_prototype.setUTCDate.apply(this._, arguments); },
  setDay: function() { fnord3_time_prototype.setUTCDay.apply(this._, arguments); },
  setFullYear: function() { fnord3_time_prototype.setUTCFullYear.apply(this._, arguments); },
  setHours: function() { fnord3_time_prototype.setUTCHours.apply(this._, arguments); },
  setMilliseconds: function() { fnord3_time_prototype.setUTCMilliseconds.apply(this._, arguments); },
  setMinutes: function() { fnord3_time_prototype.setUTCMinutes.apply(this._, arguments); },
  setMonth: function() { fnord3_time_prototype.setUTCMonth.apply(this._, arguments); },
  setSeconds: function() { fnord3_time_prototype.setUTCSeconds.apply(this._, arguments); },
  setTime: function() { fnord3_time_prototype.setTime.apply(this._, arguments); }
};

var fnord3_time_prototype = Date.prototype;
fnord3.time.format = function(template) {
  var n = template.length;

  function format(date) {
    var string = [],
        i = -1,
        j = 0,
        c,
        f;
    while (++i < n) {
      if (template.charCodeAt(i) == 37) {
        string.push(
            template.substring(j, i),
            (f = fnord3_time_formats[c = template.charAt(++i)])
            ? f(date) : c);
        j = i + 1;
      }
    }
    string.push(template.substring(j, i));
    return string.join("");
  }

  format.parse = function(string) {
    var d = {y: 1900, m: 0, d: 1, H: 0, M: 0, S: 0, L: 0},
        i = fnord3_time_parse(d, template, string, 0);
    if (i != string.length) return null;

    // The am-pm flag is 0 for AM, and 1 for PM.
    if ("p" in d) d.H = d.H % 12 + d.p * 12;

    var date = new fnord3_time();
    date.setFullYear(d.y, d.m, d.d);
    date.setHours(d.H, d.M, d.S, d.L);
    return date;
  };

  format.toString = function() {
    return template;
  };

  return format;
};

function fnord3_time_parse(date, template, string, j) {
  var c,
      p,
      i = 0,
      n = template.length,
      m = string.length;
  while (i < n) {
    if (j >= m) return -1;
    c = template.charCodeAt(i++);
    if (c == 37) {
      p = fnord3_time_parsers[template.charAt(i++)];
      if (!p || ((j = p(date, string, j)) < 0)) return -1;
    } else if (c != string.charCodeAt(j++)) {
      return -1;
    }
  }
  return j;
}

var fnord3_time_zfill2 = fnord3.format("02d"),
    fnord3_time_zfill3 = fnord3.format("03d"),
    fnord3_time_zfill4 = fnord3.format("04d"),
    fnord3_time_sfill2 = fnord3.format("2d");

var fnord3_time_formats = {
  a: function(d) { return fnord3_time_weekdays[d.getDay()].substring(0, 3); },
  A: function(d) { return fnord3_time_weekdays[d.getDay()]; },
  b: function(d) { return fnord3_time_months[d.getMonth()].substring(0, 3); },
  B: function(d) { return fnord3_time_months[d.getMonth()]; },
  c: fnord3.time.format("%a %b %e %H:%M:%S %Y"),
  d: function(d) { return fnord3_time_zfill2(d.getDate()); },
  e: function(d) { return fnord3_time_sfill2(d.getDate()); },
  H: function(d) { return fnord3_time_zfill2(d.getHours()); },
  I: function(d) { return fnord3_time_zfill2(d.getHours() % 12 || 12); },
  j: function(d) { return fnord3_time_zfill3(1 + fnord3.time.dayOfYear(d)); },
  L: function(d) { return fnord3_time_zfill3(d.getMilliseconds()); },
  m: function(d) { return fnord3_time_zfill2(d.getMonth() + 1); },
  M: function(d) { return fnord3_time_zfill2(d.getMinutes()); },
  p: function(d) { return d.getHours() >= 12 ? "PM" : "AM"; },
  S: function(d) { return fnord3_time_zfill2(d.getSeconds()); },
  U: function(d) { return fnord3_time_zfill2(fnord3.time.sundayOfYear(d)); },
  w: function(d) { return d.getDay(); },
  W: function(d) { return fnord3_time_zfill2(fnord3.time.mondayOfYear(d)); },
  x: fnord3.time.format("%m/%d/%y"),
  X: fnord3.time.format("%H:%M:%S"),
  y: function(d) { return fnord3_time_zfill2(d.getFullYear() % 100); },
  Y: function(d) { return fnord3_time_zfill4(d.getFullYear() % 10000); },
  Z: fnord3_time_zone,
  "%": function(d) { return "%"; }
};

var fnord3_time_parsers = {
  a: fnord3_time_parseWeekdayAbbrev,
  A: fnord3_time_parseWeekday,
  b: fnord3_time_parseMonthAbbrev,
  B: fnord3_time_parseMonth,
  c: fnord3_time_parseLocaleFull,
  d: fnord3_time_parseDay,
  e: fnord3_time_parseDay,
  H: fnord3_time_parseHour24,
  I: fnord3_time_parseHour24,
  // j: function(d, s, i) { /*TODO day of year [001,366] */ return i; },
  L: fnord3_time_parseMilliseconds,
  m: fnord3_time_parseMonthNumber,
  M: fnord3_time_parseMinutes,
  p: fnord3_time_parseAmPm,
  S: fnord3_time_parseSeconds,
  // U: function(d, s, i) { /*TODO week number (sunday) [00,53] */ return i; },
  // w: function(d, s, i) { /*TODO weekday [0,6] */ return i; },
  // W: function(d, s, i) { /*TODO week number (monday) [00,53] */ return i; },
  x: fnord3_time_parseLocaleDate,
  X: fnord3_time_parseLocaleTime,
  y: fnord3_time_parseYear,
  Y: fnord3_time_parseFullYear
  // ,
  // Z: function(d, s, i) { /*TODO time zone */ return i; },
  // "%": function(d, s, i) { /*TODO literal % */ return i; }
};

// Note: weekday is validated, but does not set the date.
function fnord3_time_parseWeekdayAbbrev(date, string, i) {
  return fnord3_time_weekdayAbbrevRe.test(string.substring(i, i += 3)) ? i : -1;
}

// Note: weekday is validated, but does not set the date.
function fnord3_time_parseWeekday(date, string, i) {
  fnord3_time_weekdayRe.lastIndex = 0;
  var n = fnord3_time_weekdayRe.exec(string.substring(i, i + 10));
  return n ? i += n[0].length : -1;
}

var fnord3_time_weekdayAbbrevRe = /^(?:sun|mon|tue|wed|thu|fri|sat)/i,
    fnord3_time_weekdayRe = /^(?:Sunday|Monday|Tuesday|Wednesday|Thursday|Friday|Saturday)/i,
    fnord3_time_weekdays = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];

function fnord3_time_parseMonthAbbrev(date, string, i) {
  var n = fnord3_time_monthAbbrevLookup.get(string.substring(i, i += 3).toLowerCase());
  return n == null ? -1 : (date.m = n, i);
}

var fnord3_time_monthAbbrevLookup = fnord3.map({
  jan: 0,
  feb: 1,
  mar: 2,
  apr: 3,
  may: 4,
  jun: 5,
  jul: 6,
  aug: 7,
  sep: 8,
  oct: 9,
  nov: 10,
  dec: 11
});

function fnord3_time_parseMonth(date, string, i) {
  fnord3_time_monthRe.lastIndex = 0;
  var n = fnord3_time_monthRe.exec(string.substring(i, i + 12));
  return n ? (date.m = fnord3_time_monthLookup.get(n[0].toLowerCase()), i += n[0].length) : -1;
}

var fnord3_time_monthRe = /^(?:January|February|March|April|May|June|July|August|September|October|November|December)/ig;

var fnord3_time_monthLookup = fnord3.map({
  january: 0,
  february: 1,
  march: 2,
  april: 3,
  may: 4,
  june: 5,
  july: 6,
  august: 7,
  september: 8,
  october: 9,
  november: 10,
  december: 11
});

var fnord3_time_months = [
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
];

function fnord3_time_parseLocaleFull(date, string, i) {
  return fnord3_time_parse(date, fnord3_time_formats.c.toString(), string, i);
}

function fnord3_time_parseLocaleDate(date, string, i) {
  return fnord3_time_parse(date, fnord3_time_formats.x.toString(), string, i);
}

function fnord3_time_parseLocaleTime(date, string, i) {
  return fnord3_time_parse(date, fnord3_time_formats.X.toString(), string, i);
}

function fnord3_time_parseFullYear(date, string, i) {
  fnord3_time_numberRe.lastIndex = 0;
  var n = fnord3_time_numberRe.exec(string.substring(i, i + 4));
  return n ? (date.y = +n[0], i += n[0].length) : -1;
}

function fnord3_time_parseYear(date, string, i) {
  fnord3_time_numberRe.lastIndex = 0;
  var n = fnord3_time_numberRe.exec(string.substring(i, i + 2));
  return n ? (date.y = fnord3_time_century() + +n[0], i += n[0].length) : -1;
}

function fnord3_time_century() {
  return ~~(new Date().getFullYear() / 1000) * 1000;
}

function fnord3_time_parseMonthNumber(date, string, i) {
  fnord3_time_numberRe.lastIndex = 0;
  var n = fnord3_time_numberRe.exec(string.substring(i, i + 2));
  return n ? (date.m = n[0] - 1, i += n[0].length) : -1;
}

function fnord3_time_parseDay(date, string, i) {
  fnord3_time_numberRe.lastIndex = 0;
  var n = fnord3_time_numberRe.exec(string.substring(i, i + 2));
  return n ? (date.d = +n[0], i += n[0].length) : -1;
}

// Note: we don't validate that the hour is in the range [0,23] or [1,12].
function fnord3_time_parseHour24(date, string, i) {
  fnord3_time_numberRe.lastIndex = 0;
  var n = fnord3_time_numberRe.exec(string.substring(i, i + 2));
  return n ? (date.H = +n[0], i += n[0].length) : -1;
}

function fnord3_time_parseMinutes(date, string, i) {
  fnord3_time_numberRe.lastIndex = 0;
  var n = fnord3_time_numberRe.exec(string.substring(i, i + 2));
  return n ? (date.M = +n[0], i += n[0].length) : -1;
}

function fnord3_time_parseSeconds(date, string, i) {
  fnord3_time_numberRe.lastIndex = 0;
  var n = fnord3_time_numberRe.exec(string.substring(i, i + 2));
  return n ? (date.S = +n[0], i += n[0].length) : -1;
}

function fnord3_time_parseMilliseconds(date, string, i) {
  fnord3_time_numberRe.lastIndex = 0;
  var n = fnord3_time_numberRe.exec(string.substring(i, i + 3));
  return n ? (date.L = +n[0], i += n[0].length) : -1;
}

// Note: we don't look at the next directive.
var fnord3_time_numberRe = /\s*\d+/;

function fnord3_time_parseAmPm(date, string, i) {
  var n = fnord3_time_amPmLookup.get(string.substring(i, i += 2).toLowerCase());
  return n == null ? -1 : (date.p = n, i);
}

var fnord3_time_amPmLookup = fnord3.map({
  am: 0,
  pm: 1
});

// TODO table of time zone offset names?
function fnord3_time_zone(d) {
  var z = d.getTimezoneOffset(),
      zs = z > 0 ? "-" : "+",
      zh = ~~(Math.abs(z) / 60),
      zm = Math.abs(z) % 60;
  return zs + fnord3_time_zfill2(zh) + fnord3_time_zfill2(zm);
}
fnord3.time.format.utc = function(template) {
  var local = fnord3.time.format(template);

  function format(date) {
    try {
      fnord3_time = fnord3_time_utc;
      var utc = new fnord3_time();
      utc._ = date;
      return local(utc);
    } finally {
      fnord3_time = Date;
    }
  }

  format.parse = function(string) {
    try {
      fnord3_time = fnord3_time_utc;
      var date = local.parse(string);
      return date && date._;
    } finally {
      fnord3_time = Date;
    }
  };

  format.toString = local.toString;

  return format;
};
var fnord3_time_formatIso = fnord3.time.format.utc("%Y-%m-%dT%H:%M:%S.%LZ");

fnord3.time.format.iso = Date.prototype.toISOString ? fnord3_time_formatIsoNative : fnord3_time_formatIso;

function fnord3_time_formatIsoNative(date) {
  return date.toISOString();
}

fnord3_time_formatIsoNative.parse = function(string) {
  return new Date(string);
};

fnord3_time_formatIsoNative.toString = fnord3_time_formatIso.toString;
function fnord3_time_interval(local, step, number) {

  function round(date) {
    var d0 = local(date), d1 = offset(d0, 1);
    return date - d0 < d1 - date ? d0 : d1;
  }

  function ceil(date) {
    step(date = local(new fnord3_time(date - 1)), 1);
    return date;
  }

  function offset(date, k) {
    step(date = new fnord3_time(+date), k);
    return date;
  }

  function range(t0, t1, dt) {
    var time = ceil(t0), times = [];
    if (dt > 1) {
      while (time < t1) {
        if (!(number(time) % dt)) times.push(new Date(+time));
        step(time, 1);
      }
    } else {
      while (time < t1) times.push(new Date(+time)), step(time, 1);
    }
    return times;
  }

  function range_utc(t0, t1, dt) {
    try {
      fnord3_time = fnord3_time_utc;
      var utc = new fnord3_time_utc();
      utc._ = t0;
      return range(utc, t1, dt);
    } finally {
      fnord3_time = Date;
    }
  }

  local.floor = local;
  local.round = round;
  local.ceil = ceil;
  local.offset = offset;
  local.range = range;

  var utc = local.utc = fnord3_time_interval_utc(local);
  utc.floor = utc;
  utc.round = fnord3_time_interval_utc(round);
  utc.ceil = fnord3_time_interval_utc(ceil);
  utc.offset = fnord3_time_interval_utc(offset);
  utc.range = range_utc;

  return local;
}

function fnord3_time_interval_utc(method) {
  return function(date, k) {
    try {
      fnord3_time = fnord3_time_utc;
      var utc = new fnord3_time_utc();
      utc._ = date;
      return method(utc, k)._;
    } finally {
      fnord3_time = Date;
    }
  };
}
fnord3.time.second = fnord3_time_interval(function(date) {
  return new fnord3_time(Math.floor(date / 1e3) * 1e3);
}, function(date, offset) {
  date.setTime(date.getTime() + Math.floor(offset) * 1e3); // DST breaks setSeconds
}, function(date) {
  return date.getSeconds();
});

fnord3.time.seconds = fnord3.time.second.range;
fnord3.time.seconds.utc = fnord3.time.second.utc.range;
fnord3.time.minute = fnord3_time_interval(function(date) {
  return new fnord3_time(Math.floor(date / 6e4) * 6e4);
}, function(date, offset) {
  date.setTime(date.getTime() + Math.floor(offset) * 6e4); // DST breaks setMinutes
}, function(date) {
  return date.getMinutes();
});

fnord3.time.minutes = fnord3.time.minute.range;
fnord3.time.minutes.utc = fnord3.time.minute.utc.range;
fnord3.time.hour = fnord3_time_interval(function(date) {
  var timezone = date.getTimezoneOffset() / 60;
  return new fnord3_time((Math.floor(date / 36e5 - timezone) + timezone) * 36e5);
}, function(date, offset) {
  date.setTime(date.getTime() + Math.floor(offset) * 36e5); // DST breaks setHours
}, function(date) {
  return date.getHours();
});

fnord3.time.hours = fnord3.time.hour.range;
fnord3.time.hours.utc = fnord3.time.hour.utc.range;
fnord3.time.day = fnord3_time_interval(function(date) {
  return new fnord3_time(date.getFullYear(), date.getMonth(), date.getDate());
}, function(date, offset) {
  date.setDate(date.getDate() + offset);
}, function(date) {
  return date.getDate() - 1;
});

fnord3.time.days = fnord3.time.day.range;
fnord3.time.days.utc = fnord3.time.day.utc.range;

fnord3.time.dayOfYear = function(date) {
  var year = fnord3.time.year(date);
  return Math.floor((date - year) / 864e5 - (date.getTimezoneOffset() - year.getTimezoneOffset()) / 1440);
};
fnord3_time_weekdays.forEach(function(day, i) {
  day = day.toLowerCase();
  i = 7 - i;

  var interval = fnord3.time[day] = fnord3_time_interval(function(date) {
    (date = fnord3.time.day(date)).setDate(date.getDate() - (date.getDay() + i) % 7);
    return date;
  }, function(date, offset) {
    date.setDate(date.getDate() + Math.floor(offset) * 7);
  }, function(date) {
    var day = fnord3.time.year(date).getDay();
    return Math.floor((fnord3.time.dayOfYear(date) + (day + i) % 7) / 7) - (day !== i);
  });

  fnord3.time[day + "s"] = interval.range;
  fnord3.time[day + "s"].utc = interval.utc.range;

  fnord3.time[day + "OfYear"] = function(date) {
    var day = fnord3.time.year(date).getDay();
    return Math.floor((fnord3.time.dayOfYear(date) + (day + i) % 7) / 7);
  };
});

fnord3.time.week = fnord3.time.sunday;
fnord3.time.weeks = fnord3.time.sunday.range;
fnord3.time.weeks.utc = fnord3.time.sunday.utc.range;
fnord3.time.weekOfYear = fnord3.time.sundayOfYear;
fnord3.time.month = fnord3_time_interval(function(date) {
  return new fnord3_time(date.getFullYear(), date.getMonth(), 1);
}, function(date, offset) {
  date.setMonth(date.getMonth() + offset);
}, function(date) {
  return date.getMonth();
});

fnord3.time.months = fnord3.time.month.range;
fnord3.time.months.utc = fnord3.time.month.utc.range;
fnord3.time.year = fnord3_time_interval(function(date) {
  return new fnord3_time(date.getFullYear(), 0, 1);
}, function(date, offset) {
  date.setFullYear(date.getFullYear() + offset);
}, function(date) {
  return date.getFullYear();
});

fnord3.time.years = fnord3.time.year.range;
fnord3.time.years.utc = fnord3.time.year.utc.range;
function fnord3_time_scale(linear, methods, format) {

  function scale(x) {
    return linear(x);
  }

  scale.invert = function(x) {
    return fnord3_time_scaleDate(linear.invert(x));
  };

  scale.domain = function(x) {
    if (!arguments.length) return linear.domain().map(fnord3_time_scaleDate);
    linear.domain(x);
    return scale;
  };

  scale.nice = function(m) {
    var extent = fnord3_time_scaleExtent(scale.domain());
    return scale.domain([m.floor(extent[0]), m.ceil(extent[1])]);
  };

  scale.ticks = function(m, k) {
    var extent = fnord3_time_scaleExtent(scale.domain());
    if (typeof m !== "function") {
      var span = extent[1] - extent[0],
          target = span / m,
          i = fnord3.bisect(fnord3_time_scaleSteps, target);
      if (i == fnord3_time_scaleSteps.length) return methods.year(extent, m);
      if (!i) return linear.ticks(m).map(fnord3_time_scaleDate);
      if (Math.log(target / fnord3_time_scaleSteps[i - 1]) < Math.log(fnord3_time_scaleSteps[i] / target)) --i;
      m = methods[i];
      k = m[1];
      m = m[0].range;
    }
    return m(extent[0], new Date(+extent[1] + 1), k); // inclusive upper bound
  };

  scale.tickFormat = function() {
    return format;
  };

  scale.copy = function() {
    return fnord3_time_scale(linear.copy(), methods, format);
  };

  // TOOD expose fnord3_scale_linear_rebind?
  return fnord3.rebind(scale, linear, "range", "rangeRound", "interpolate", "clamp");
}

// TODO expose fnord3_scaleExtent?
function fnord3_time_scaleExtent(domain) {
  var start = domain[0], stop = domain[domain.length - 1];
  return start < stop ? [start, stop] : [stop, start];
}

function fnord3_time_scaleDate(t) {
  return new Date(t);
}

function fnord3_time_scaleFormat(formats) {
  return function(date) {
    var i = formats.length - 1, f = formats[i];
    while (!f[1](date)) f = formats[--i];
    return f[0](date);
  };
}

function fnord3_time_scaleSetYear(y) {
  var d = new Date(y, 0, 1);
  d.setFullYear(y); // Y2K fail
  return d;
}

function fnord3_time_scaleGetYear(d) {
  var y = d.getFullYear(),
      d0 = fnord3_time_scaleSetYear(y),
      d1 = fnord3_time_scaleSetYear(y + 1);
  return y + (d - d0) / (d1 - d0);
}

var fnord3_time_scaleSteps = [
  1e3,    // 1-second
  5e3,    // 5-second
  15e3,   // 15-second
  3e4,    // 30-second
  6e4,    // 1-minute
  3e5,    // 5-minute
  9e5,    // 15-minute
  18e5,   // 30-minute
  36e5,   // 1-hour
  108e5,  // 3-hour
  216e5,  // 6-hour
  432e5,  // 12-hour
  864e5,  // 1-day
  1728e5, // 2-day
  6048e5, // 1-week
  2592e6, // 1-month
  7776e6, // 3-month
  31536e6 // 1-year
];

var fnord3_time_scaleLocalMethods = [
  [fnord3.time.second, 1],
  [fnord3.time.second, 5],
  [fnord3.time.second, 15],
  [fnord3.time.second, 30],
  [fnord3.time.minute, 1],
  [fnord3.time.minute, 5],
  [fnord3.time.minute, 15],
  [fnord3.time.minute, 30],
  [fnord3.time.hour, 1],
  [fnord3.time.hour, 3],
  [fnord3.time.hour, 6],
  [fnord3.time.hour, 12],
  [fnord3.time.day, 1],
  [fnord3.time.day, 2],
  [fnord3.time.week, 1],
  [fnord3.time.month, 1],
  [fnord3.time.month, 3],
  [fnord3.time.year, 1]
];

var fnord3_time_scaleLocalFormats = [
  [fnord3.time.format("%Y"), function(d) { return true; }],
  [fnord3.time.format("%B"), function(d) { return d.getMonth(); }],
  [fnord3.time.format("%b %d"), function(d) { return d.getDate() != 1; }],
  [fnord3.time.format("%a %d"), function(d) { return d.getDay() && d.getDate() != 1; }],
  [fnord3.time.format("%I %p"), function(d) { return d.getHours(); }],
  [fnord3.time.format("%I:%M"), function(d) { return d.getMinutes(); }],
  [fnord3.time.format(":%S"), function(d) { return d.getSeconds(); }],
  [fnord3.time.format(".%L"), function(d) { return d.getMilliseconds(); }]
];

var fnord3_time_scaleLinear = fnord3.scale.linear(),
    fnord3_time_scaleLocalFormat = fnord3_time_scaleFormat(fnord3_time_scaleLocalFormats);

fnord3_time_scaleLocalMethods.year = function(extent, m) {
  return fnord3_time_scaleLinear.domain(extent.map(fnord3_time_scaleGetYear)).ticks(m).map(fnord3_time_scaleSetYear);
};

fnord3.time.scale = function() {
  return fnord3_time_scale(fnord3.scale.linear(), fnord3_time_scaleLocalMethods, fnord3_time_scaleLocalFormat);
};
var fnord3_time_scaleUTCMethods = fnord3_time_scaleLocalMethods.map(function(m) {
  return [m[0].utc, m[1]];
});

var fnord3_time_scaleUTCFormats = [
  [fnord3.time.format.utc("%Y"), function(d) { return true; }],
  [fnord3.time.format.utc("%B"), function(d) { return d.getUTCMonth(); }],
  [fnord3.time.format.utc("%b %d"), function(d) { return d.getUTCDate() != 1; }],
  [fnord3.time.format.utc("%a %d"), function(d) { return d.getUTCDay() && d.getUTCDate() != 1; }],
  [fnord3.time.format.utc("%I %p"), function(d) { return d.getUTCHours(); }],
  [fnord3.time.format.utc("%I:%M"), function(d) { return d.getUTCMinutes(); }],
  [fnord3.time.format.utc(":%S"), function(d) { return d.getUTCSeconds(); }],
  [fnord3.time.format.utc(".%L"), function(d) { return d.getUTCMilliseconds(); }]
];

var fnord3_time_scaleUTCFormat = fnord3_time_scaleFormat(fnord3_time_scaleUTCFormats);

function fnord3_time_scaleUTCSetYear(y) {
  var d = new Date(Date.UTC(y, 0, 1));
  d.setUTCFullYear(y); // Y2K fail
  return d;
}

function fnord3_time_scaleUTCGetYear(d) {
  var y = d.getUTCFullYear(),
      d0 = fnord3_time_scaleUTCSetYear(y),
      d1 = fnord3_time_scaleUTCSetYear(y + 1);
  return y + (d - d0) / (d1 - d0);
}

fnord3_time_scaleUTCMethods.year = function(extent, m) {
  return fnord3_time_scaleLinear.domain(extent.map(fnord3_time_scaleUTCGetYear)).ticks(m).map(fnord3_time_scaleUTCSetYear);
};

fnord3.time.scale.utc = function() {
  return fnord3_time_scale(fnord3.scale.linear(), fnord3_time_scaleUTCMethods, fnord3_time_scaleUTCFormat);
};
})();
// FnordMetric UI
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

var FnordMetric = (function(pre){

  var version = "1.2.9";

  var wsAddress, socket, currentNamespace,
     continuations = {},
     continuation = false,
     stack = [],
     widgets = {},
     enterprise = false;

  function setup(opts) {
    if (typeof $ == 'undefined') {
      console.log("error: FnordMetric requires jQuery 1.6.2+");
      return;
    }

    currentNamespace = opts.namespace;
    wsAddress = "ws://" + opts.address + "/stream";

    $(document).ready(function(){
      connect();
    });
  }

  function connect() {
    if (enterprise)
      socket = new WebSocket(wsAddress, "fnordmetric_enterprise");
    else
      socket = new WebSocket(wsAddress);

    socket.onmessage = onSocketMessage;
    socket.onclose = onSocketClose;
    socket.onopen = onSocketOpen;
  }

  function publish(obj) {
    if (!obj.namespace)
      obj.namespace = currentNamespace;

    socket.send(JSON.stringify(obj));
  }

  function refresh(elem) {
    var elem = $(elem);

    var widget_key = elem.attr('data-widget-key');
    var widget_type = elem.attr('data-fnordmetric');

    if (!widget_type)
      return console.log("[FnordMetric] element is missing the data-fnordmetric attribute");

    if (widget_key && widgets[widget_key]) {
      widgets[widget_key].destroy();
      delete widgets[widget_key];
    }

    widget_key = FnordMetric.util.generateUUID();
    elem.attr('data-widget-key', widget_key);

    widgets[widget_key] = FnordMetric.widgets[widget_type](elem);
    widgets[widget_key].init();
  }

  function resize() {
    for (widget_key in widgets)
      if (widgets[widget_key].resize)
        widgets[widget_key].resize();
  }

  function onSocketMessage(raw) {
    if (enterprise) {
      var data = raw.data;

      if (data.substr(0,5) == "ERROR")
        return console.log("[FnordMetric] error: " + data.substr(6));

      else if (continuation) {
        continuation(data);
        continuation = false;

        if (stack.length > 0) {
          var nxt = stack.shift();
          execute(nxt[0], nxt[1])
        }
      }

    } else {
      var n, evt = JSON.parse(raw.data);

      if (evt.error)
        return console.log("[FnordMetric] error: " + evt.error);

      if (evt.widget_key && continuations[evt.widget_key])
        continuations[evt.widget_key].apply(evt);

      else if (evt.widget_key && widgets[evt.widget_key])
        widgets[evt.widget_key].send(evt);
    }
  }

  function onSocketOpen() {
    console.log("[FnordMetric] connected...");

    $("*[data-fnordmetric]").each(function(n, e){
      if (!$(e).attr('data-widget-key'))
        refresh(e);
    });
  }

  function onSocketClose(e) {
    if (e.code = 1003 && e.reason == "fnordmetric_enterprise") {
      console.log("[FnordMetric] switching to fnordmetric enterprise protocol")
      enterprise = true;

      $("*[data-fnordmetric]").each(function(n, e){
        $(e).attr('data-widget-key', null)
      });

      window.setTimeout(connect, 10);
    } else {
      console.log("[FnordMetric] socket closed");
      window.setTimeout(connect, 1000);
    }
  }

  function execute(cmd, cb) {
    if (continuation === false) {
      socket.send(cmd);
      continuation = cb;
    } else {
      stack.push([cmd, cb]);
    }
  }

  function values_in(gauges, since, until, callback) {
    var tsince = FnordMetric.util.parseTime(since),
        tuntil = FnordMetric.util.parseTime(until);

    if (enterprise) {
      var all_resp = {};

      function values_in_fetch_next(gauges) {
        var this_resp = gauges.shift();

        if (typeof this_resp == 'undefined')
          return;

        var cmd = "VALUESIN " + this_resp + " " + since + " " + until;

        execute(cmd, function(resp) {
          var vals = {},
              parts = resp.split(" ");

          if (parts[0] != "null")
            for (ind in parts) {
              var tuple = parts[ind].split(":");
              vals[parseInt(parseInt(tuple[0], 10) / 1000, 10)] = tuple[1];
            }

          all_resp[this_resp] = vals;

          if (gauges.length == 0)
            callback.apply(FnordMetric.util.zeroFill(all_resp, tsince, tuntil));
          else
            values_in_fetch_next(gauges);
        });
      }

      values_in_fetch_next(gauges);
    }

    else {
      var txid = FnordMetric.util.generateUUID();

      continuations[txid] = function(){
        var result = {};

        for (ind in this.gauges)
          result[this.gauges[ind].key] = this.gauges[ind].vals;

        callback.apply(result);
      }

      FnordMetric.publish({
        "type": "widget_request",
        "klass": "generic",
        "gauges": gauges,
        "cmd": "values_at",
        "since": since,
        "until": until,
        "widget_key": txid
      });
    }
  }

  function value_at(gauge, at, callback) {
    if (enterprise) {
      execute(
        "VALUEAT " + gauge + " " + at,
        function(resp) {
          callback.apply({ "value": eval(resp) });
        }
      );
    }

    else {
      var txid = FnordMetric.util.generateUUID();

      continuations[txid] = function(){
        callback.apply({ "value": this.value });
      }

      FnordMetric.publish({
        "type": "widget_request",
        "klass": "generic",
        "cmd": "values_for",
        "gauge": gauge,
        "at": at,
        "widget_key": txid
      })
    }
  }

  return {
    setup: setup,
    publish: publish,
    refresh: refresh,
    resize: resize,
    execute: execute,
    value_at: value_at,
    values_in: values_in
  };

})(FnordMetric);
/* this is a modified version of shutterstock's rickshaw.js (MIT license like FnordMetric) */

FnordMetric.rickshaw = {

  namespace: function(namespace, obj) {

    var parts = namespace.split('.');

    // for rudimentary compatibility w/ node
    var root = typeof global != 'undefined' ? global : window;

    var parent = root.FnordMetric;

    for(var i = 1, length = parts.length; i < length; i++) {
      currentPart = parts[i];
      parent[currentPart] = parent[currentPart] || {};
      parent = parent[currentPart];
    }
    return parent;
  },

  keys: function(obj) {
    var keys = [];
    for (var key in obj) keys.push(key);
    return keys;
  },

  extend: function(destination, source) {

    for (var property in source) {
      destination[property] = source[property];
    }
    return destination;
  }
};

/* Adapted from https://github.com/Jakobo/PTClass */

/*
Copyright (c) 2005-2010 Sam Stephenson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
/* Based on Alex Arnell's inheritance implementation. */
/** section: Language
 * class Class
 *
 *  Manages Prototype's class-based OOP system.
 *
 *  Refer to Prototype's web site for a [tutorial on classes and
 *  inheritance](http://prototypejs.org/learn/class-inheritance).
**/
(function(globalContext) {
/* ------------------------------------ */
/* Import from object.js                */
/* ------------------------------------ */
var _toString = Object.prototype.toString,
    NULL_TYPE = 'Null',
    UNDEFINED_TYPE = 'Undefined',
    BOOLEAN_TYPE = 'Boolean',
    NUMBER_TYPE = 'Number',
    STRING_TYPE = 'String',
    OBJECT_TYPE = 'Object',
    FUNCTION_CLASS = '[object Function]';
function isFunction(object) {
  return _toString.call(object) === FUNCTION_CLASS;
}
function extend(destination, source) {
  for (var property in source) if (source.hasOwnProperty(property)) // modify protect primitive slaughter
    destination[property] = source[property];
  return destination;
}
function keys(object) {
  if (Type(object) !== OBJECT_TYPE) { throw new TypeError(); }
  var results = [];
  for (var property in object) {
    if (object.hasOwnProperty(property)) {
      results.push(property);
    }
  }
  return results;
}
function Type(o) {
  switch(o) {
    case null: return NULL_TYPE;
    case (void 0): return UNDEFINED_TYPE;
  }
  var type = typeof o;
  switch(type) {
    case 'boolean': return BOOLEAN_TYPE;
    case 'number':  return NUMBER_TYPE;
    case 'string':  return STRING_TYPE;
  }
  return OBJECT_TYPE;
}
function isUndefined(object) {
  return typeof object === "undefined";
}
/* ------------------------------------ */
/* Import from Function.js              */
/* ------------------------------------ */
var slice = Array.prototype.slice;
function argumentNames(fn) {
  var names = fn.toString().match(/^[\s\(]*function[^(]*\(([^)]*)\)/)[1]
    .replace(/\/\/.*?[\r\n]|\/\*(?:.|[\r\n])*?\*\//g, '')
    .replace(/\s+/g, '').split(',');
  return names.length == 1 && !names[0] ? [] : names;
}
function wrap(fn, wrapper) {
  var __method = fn;
  return function() {
    var a = update([bind(__method, this)], arguments);
    return wrapper.apply(this, a);
  }
}
function update(array, args) {
  var arrayLength = array.length, length = args.length;
  while (length--) array[arrayLength + length] = args[length];
  return array;
}
function merge(array, args) {
  array = slice.call(array, 0);
  return update(array, args);
}
function bind(fn, context) {
  if (arguments.length < 2 && isUndefined(arguments[0])) return this;
  var __method = fn, args = slice.call(arguments, 2);
  return function() {
    var a = merge(args, arguments);
    return __method.apply(context, a);
  }
}

/* ------------------------------------ */
/* Import from Prototype.js             */
/* ------------------------------------ */
var emptyFunction = function(){};

var Class = (function() {

  // Some versions of JScript fail to enumerate over properties, names of which
  // correspond to non-enumerable properties in the prototype chain
  var IS_DONTENUM_BUGGY = (function(){
    for (var p in { toString: 1 }) {
      // check actual property name, so that it works with augmented Object.prototype
      if (p === 'toString') return false;
    }
    return true;
  })();

  function subclass() {};
  function create() {
    var parent = null, properties = [].slice.apply(arguments);
    if (isFunction(properties[0]))
      parent = properties.shift();

    function klass() {
      this.initialize.apply(this, arguments);
    }

    extend(klass, Class.Methods);
    klass.superclass = parent;
    klass.subclasses = [];

    if (parent) {
      subclass.prototype = parent.prototype;
      klass.prototype = new subclass;
      try { parent.subclasses.push(klass) } catch(e) {}
    }

    for (var i = 0, length = properties.length; i < length; i++)
      klass.addMethods(properties[i]);

    if (!klass.prototype.initialize)
      klass.prototype.initialize = emptyFunction;

    klass.prototype.constructor = klass;
    return klass;
  }

  function addMethods(source) {
    var ancestor   = this.superclass && this.superclass.prototype,
        properties = keys(source);

    // IE6 doesn't enumerate `toString` and `valueOf` (among other built-in `Object.prototype`) properties,
    // Force copy if they're not Object.prototype ones.
    // Do not copy other Object.prototype.* for performance reasons
    if (IS_DONTENUM_BUGGY) {
      if (source.toString != Object.prototype.toString)
        properties.push("toString");
      if (source.valueOf != Object.prototype.valueOf)
        properties.push("valueOf");
    }

    for (var i = 0, length = properties.length; i < length; i++) {
      var property = properties[i], value = source[property];
      if (ancestor && isFunction(value) &&
          argumentNames(value)[0] == "$super") {
        var method = value;
        value = wrap((function(m) {
          return function() { return ancestor[m].apply(this, arguments); };
        })(property), method);

        value.valueOf = bind(method.valueOf, method);
        value.toString = bind(method.toString, method);
      }
      this.prototype[property] = value;
    }

    return this;
  }

  return {
    create: create,
    Methods: {
      addMethods: addMethods
    }
  };
})();

if (globalContext.exports) {
  globalContext.exports.Class = Class;
}
else {
  globalContext.Class = Class;
}
})(FnordMetric.rickshaw);
FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Compat.ClassList');

FnordMetric.rickshaw.Compat.ClassList = function() {

  /* adapted from http://purl.eligrey.com/github/classList.js/blob/master/classList.js */

  if (typeof document !== "undefined" && !("classList" in document.createElement("a"))) {

  (function (view) {

  "use strict";

  var
      classListProp = "classList"
    , protoProp = "prototype"
    , elemCtrProto = (view.HTMLElement || view.Element)[protoProp]
    , objCtr = Object
    , strTrim = String[protoProp].trim || function () {
      return this.replace(/^\s+|\s+$/g, "");
    }
    , arrIndexOf = Array[protoProp].indexOf || function (item) {
      var
          i = 0
        , len = this.length
      ;
      for (; i < len; i++) {
        if (i in this && this[i] === item) {
          return i;
        }
      }
      return -1;
    }
    // Vendors: please allow content code to instantiate DOMExceptions
    , DOMEx = function (type, message) {
      this.name = type;
      this.code = DOMException[type];
      this.message = message;
    }
    , checkTokenAndGetIndex = function (classList, token) {
      if (token === "") {
        throw new DOMEx(
            "SYNTAX_ERR"
          , "An invalid or illegal string was specified"
        );
      }
      if (/\s/.test(token)) {
        throw new DOMEx(
            "INVALID_CHARACTER_ERR"
          , "String contains an invalid character"
        );
      }
      return arrIndexOf.call(classList, token);
    }
    , ClassList = function (elem) {
      var
          trimmedClasses = strTrim.call(elem.className)
        , classes = trimmedClasses ? trimmedClasses.split(/\s+/) : []
        , i = 0
        , len = classes.length
      ;
      for (; i < len; i++) {
        this.push(classes[i]);
      }
      this._updateClassName = function () {
        elem.className = this.toString();
      };
    }
    , classListProto = ClassList[protoProp] = []
    , classListGetter = function () {
      return new ClassList(this);
    }
  ;
  // Most DOMException implementations don't allow calling DOMException's toString()
  // on non-DOMExceptions. Error's toString() is sufficient here.
  DOMEx[protoProp] = Error[protoProp];
  classListProto.item = function (i) {
    return this[i] || null;
  };
  classListProto.contains = function (token) {
    token += "";
    return checkTokenAndGetIndex(this, token) !== -1;
  };
  classListProto.add = function (token) {
    token += "";
    if (checkTokenAndGetIndex(this, token) === -1) {
      this.push(token);
      this._updateClassName();
    }
  };
  classListProto.remove = function (token) {
    token += "";
    var index = checkTokenAndGetIndex(this, token);
    if (index !== -1) {
      this.splice(index, 1);
      this._updateClassName();
    }
  };
  classListProto.toggle = function (token) {
    token += "";
    if (checkTokenAndGetIndex(this, token) === -1) {
      this.add(token);
    } else {
      this.remove(token);
    }
  };
  classListProto.toString = function () {
    return this.join(" ");
  };

  if (objCtr.defineProperty) {
    var classListPropDesc = {
        get: classListGetter
      , enumerable: true
      , configurable: true
    };
    try {
      objCtr.defineProperty(elemCtrProto, classListProp, classListPropDesc);
    } catch (ex) { // IE 8 doesn't support enumerable:true
      if (ex.number === -0x7FF5EC54) {
        classListPropDesc.enumerable = false;
        objCtr.defineProperty(elemCtrProto, classListProp, classListPropDesc);
      }
    }
  } else if (objCtr[protoProp].__defineGetter__) {
    elemCtrProto.__defineGetter__(classListProp, classListGetter);
  }

  }(self));

  }
};

if ( (typeof RICKSHAW_NO_COMPAT !== "undefined" && !RICKSHAW_NO_COMPAT) || typeof RICKSHAW_NO_COMPAT === "undefined") {
  new FnordMetric.rickshaw.Compat.ClassList();
}
FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Graph');

FnordMetric.rickshaw.Graph = function(args) {

  this.element = args.element;
  this.series = args.series;

  this.defaults = {
    interpolation: 'cardinal',
    offset: 'zero',
    min: undefined,
    max: undefined,
  };

  FnordMetric.rickshaw.keys(this.defaults).forEach( function(k) {
    this[k] = args[k] || this.defaults[k];
  }, this );

  this.window = {};

  this.updateCallbacks = [];

  var self = this;

  this.initialize = function(args) {

    this.validateSeries(args.series);

    this.series.active = function() { return self.series.filter( function(s) { return !s.disabled } ) };

    this.setSize({ width: args.width, height: args.height });

    this.element.classList.add('fnordmetric_graph');
    this.vis = fnord3.select(this.element)
      .append("svg:svg")
      .attr('width', this.width)
      .attr('height', this.height);

    var renderers = [
      FnordMetric.rickshaw.Graph.Renderer.Stack,
      FnordMetric.rickshaw.Graph.Renderer.Line,
      FnordMetric.rickshaw.Graph.Renderer.Bar,
      FnordMetric.rickshaw.Graph.Renderer.Area
    ];

    renderers.forEach( function(r) {
      if (!r) return;
      self.registerRenderer(new r( { graph: self } ));
    } );

    this.setRenderer(args.renderer || 'stack', args);
    this.discoverRange();
  };

  this.validateSeries = function(series) {

    if (!(series instanceof Array) && !(series instanceof FnordMetric.rickshaw.Series)) {
      var seriesSignature = Object.prototype.toString.apply(series);
      throw "series is not an array: " + seriesSignature;
    }

    var pointsCount;

    series.forEach( function(s) {

      if (!(s instanceof Object)) {
        throw "series element is not an object: " + s;
      }
      if (!(s.data)) {
        throw "series has no data: " + JSON.stringify(s);
      }
      if (!(s.data instanceof Array)) {
        throw "series data is not an array: " + JSON.stringify(s.data);
      }

      pointsCount = pointsCount || s.data.length;

      if (pointsCount && s.data.length != pointsCount) {
        throw "series cannot have differing numbers of points: " +
          pointsCount + " vs " + s.data.length + "; see FnordMetric.rickshaw.Series.zeroFill()";
      }

      var dataTypeX = typeof s.data[0].x;
      var dataTypeY = typeof s.data[0].y;

      if (dataTypeX != 'number' || dataTypeY != 'number') {
        throw "x and y properties of points should be numbers instead of " +
          dataTypeX + " and " + dataTypeY;
      }
    } );
  };

  this.dataDomain = function() {

    // take from the first series
    var data = this.series[0].data;

    return [ data[0].x, data.slice(-1).shift().x ];

  };

  this.discoverRange = function() {

    var domain = this.renderer.domain();

    this.x = fnord3.scale.linear().domain(domain.x).range([0, this.width]);

    this.y = fnord3.scale.linear().domain(domain.y).range([this.height, 0]);
    this.y.magnitude = fnord3.scale.linear().domain(domain.y).range([0, this.height]);

  };

  this.render = function() {

    var stackedData = this.stackData();
    this.discoverRange();

    this.renderer.render();

    this.updateCallbacks.forEach( function(callback) {
      callback();
    } );
  };

  this.update = this.render;

  this.stackData = function() {

    var data = this.series.active()
      .map( function(d) { return d.data } )
      .map( function(d) { return d.filter( function(d) { return this._slice(d) }, this ) }, this);

    this.stackData.hooks.data.forEach( function(entry) {
      data = entry.f.apply(self, [data]);
    } );

    var layout = fnord3.layout.stack();
    layout.offset( self.offset );

    var stackedData = layout(data);

    this.stackData.hooks.after.forEach( function(entry) {
      stackedData = entry.f.apply(self, [data]);
    } );

    var i = 0;
    this.series.forEach( function(series) {
      if (series.disabled) return;
      series.stack = stackedData[i++];
    } );

    this.stackedData = stackedData;
    return stackedData;
  };

  this.stackData.hooks = { data: [], after: [] };

  this._slice = function(d) {

    if (this.window.xMin || this.window.xMax) {

      var isInRange = true;

      if (this.window.xMin && d.x < this.window.xMin) isInRange = false;
      if (this.window.xMax && d.x > this.window.xMax) isInRange = false;

      return isInRange;
    }

    return true;
  };

  this.onUpdate = function(callback) {
    this.updateCallbacks.push(callback);
  };

  this.registerRenderer = function(renderer) {
    this._renderers = this._renderers || {};
    this._renderers[renderer.name] = renderer;
  };

  this.configure = function(args) {

    if (args.width || args.height) {
      this.setSize(args);
    }

    FnordMetric.rickshaw.keys(this.defaults).forEach( function(k) {
      this[k] = args[k] || this.defaults[k];
    }, this );

    this.setRenderer(args.renderer || graph.renderer.name, args);
  };

  this.setRenderer = function(name, args) {

    if (!this._renderers[name]) {
      throw "couldn't find renderer " + name;
    }
    this.renderer = this._renderers[name];

    if (typeof args == 'object') {
      this.renderer.configure(args);
    }
  };

  this.setSize = function(args) {

    args = args || {};

    if (typeof window !== undefined) {
      var style = window.getComputedStyle(this.element, null);
      var elementWidth = parseInt(style.getPropertyValue('width'));
      var elementHeight = parseInt(style.getPropertyValue('height'));
    }

    this.width = args.width || elementWidth || 400;
    this.height = args.height || elementHeight || 250;

    this.vis && this.vis
      .attr('width', this.width)
      .attr('height', this.height);
  }

  this.initialize(args);
};

FnordMetric.rickshaw.namespace("FnordMetric.rickshaw.Color.Palette");

FnordMetric.rickshaw.Color.Palette = function(args) {

  var color = new FnordMetric.rickshaw.Fixtures.Color();

  args = args || {};
  this.schemes = {};

  this.scheme = color.schemes[args.scheme] || args.scheme || color.schemes.colorwheel;
  this.runningIndex = 0;

  this.color = function(key) {
    return this.scheme[key] || this.scheme[this.runningIndex++] || '#808080';
  };
};
FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Graph.Axis.Time');

FnordMetric.rickshaw.Graph.Axis.Time = function(args) {

  var self = this;

  this.graph = args.graph;
  this.elements = [];
  this.ticksTreatment = args.ticksTreatment || 'plain';
  this.fixedTimeUnit = args.timeUnit;

  this.appropriateTimeUnit = function() {
    var unit;
    var units = [
      { seconds: (86400) },
      { seconds: (3600 * 6) },
      { seconds: (3600) },
      { seconds: (60 * 15) },
      { seconds: (60) },
      { seconds: (15) },
      { seconds: (1) }
    ]

    var domain = this.graph.x.domain();
    var rangeSeconds = domain[1] - domain[0];

    units.forEach( function(u) {
      if (Math.floor(rangeSeconds / u.seconds) >= 2) {
        unit = unit || u;
      }
    } );

    return (unit || units[units.length - 1]);
  };

  this.tickOffsets = function() {

    var domain = this.graph.x.domain();

    var unit = this.fixedTimeUnit || this.appropriateTimeUnit();
    var count = Math.ceil((domain[1] - domain[0]) / unit.seconds);

    var runningTick = domain[0];

    var offsets = [];

    for (var i = 0; i < count; i++) {

      tickValue = (Math.ceil(runningTick / unit.seconds) * unit.seconds);
      runningTick = tickValue + unit.seconds / 2;

      offsets.push( { value: tickValue, unit: unit } );
    }

    return offsets;
  };

  this.render = function() {
    this.elements.forEach( function(e) {
      if (e.parentNode)
        e.parentNode.removeChild(e);
    } );

    this.elements = [];

    var offsets = this.tickOffsets();
    var domain = this.graph.x.domain();
    var rangeSeconds = domain[1] - domain[0];

    offsets.forEach( function(o) {

      if (self.graph.x(o.value) > self.graph.x.range()[1]) return;

      var element = document.createElement('div');
      element.style.left = self.graph.x(o.value) + 'px';
      element.classList.add('x_tick');
      element.classList.add(self.ticksTreatment);

      var title = document.createElement('div');
      title.classList.add('title');
      title.innerHTML = FnordMetric.util.dateFormatWithRange(o.value, rangeSeconds);
      element.appendChild(title);

      self.graph.element.appendChild(element);
      self.elements.push(element);

    } );
  };

  this.graph.onUpdate( function() { self.render() } );
};

FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Graph.Axis.Y');

FnordMetric.rickshaw.Graph.Axis.Y = function(args) {

  var self = this;
  var berthRate = 0.10;

  this.initialize = function(args) {

    this.graph = args.graph;
    this.orientation = args.orientation || 'right';

        var pixelsPerTick = 60;

    if(Math.floor(this.graph.height / pixelsPerTick) > 6){
      pixelsPerTick = Math.floor(this.graph.height / 6);
    }

    this.ticks = args.ticks || Math.floor(this.graph.height / pixelsPerTick);
    this.tickSize = args.tickSize || 4;
    this.ticksTreatment = args.ticksTreatment || 'plain';

    if (args.element) {

      this.element = args.element;
      this.vis = fnord3.select(args.element)
        .append("svg:svg")
        .attr('class', 'fnordmetric_graph y_axis');

      this.element = this.vis[0][0];
      this.element.style.position = 'relative';

      this.setSize({ width: args.width, height: args.height });

    } else {
      this.vis = this.graph.vis;
    }

    this.graph.onUpdate( function() { self.render() } );
  };

  this.setSize = function(args) {

    args = args || {};

    if (!this.element) return;

    if (typeof window !== undefined) {

      var style = window.getComputedStyle(this.element, null);
      var elementWidth = parseInt(style.getPropertyValue('width'));

      if (!args.auto) {
        var elementHeight = parseInt(style.getPropertyValue('height'));
      }
    }

    this.width = args.width || elementWidth || this.graph.width * berthRate;
    this.height = args.height || elementHeight || this.graph.height;

    this.vis
      .attr('width', this.width)
      .attr('height', this.height * (1 + berthRate));

    var berth = this.height * berthRate;
    this.element.style.top = -1 * berth + 'px';
    this.element.style.paddingTop = berth + 'px';
  };

  this.render = function() {

    if (this.graph.height !== this._renderHeight) this.setSize({ auto: true });

    var axis = fnord3.svg.axis().scale(this.graph.y).orient(this.orientation);
    axis.tickFormat( args.tickFormat || function(y) { return y } );

    if (this.orientation == 'left') {
      var transform = 'translate(' + this.width + ', 0)';
    }

    if (this.element) {
      this.vis.selectAll('*').remove();
    }

    this.vis
      .append("svg:g")
      .attr("class", ["y_ticks", this.ticksTreatment].join(" "))
      .attr("transform", transform)
      .call(axis.ticks(this.ticks).tickSubdivide(0).tickSize(this.tickSize))

    var gridSize = (this.orientation == 'right' ? 1 : -1) * this.graph.width;

    this.graph.vis
      .append("svg:g")
      .attr("class", "y_grid")
      .call(axis.ticks(this.ticks).tickSubdivide(0).tickSize(gridSize));

    this._renderHeight = this.graph.height;
  };

  this.initialize(args);
};

FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Graph.Behavior.Series.Highlight');

FnordMetric.rickshaw.Graph.Behavior.Series.Highlight = function(args) {

  this.graph = args.graph;
  this.legend = args.legend;

  var self = this;

  var colorSafe = {};

  this.addHighlightEvents = function (l) {
    l.element.addEventListener( 'mouseover', function(e) {

      self.legend.lines.forEach( function(line) {
        if (l === line) return;
        colorSafe[line.series.name] = colorSafe[line.series.name] || line.series.color;
        line.series.color = fnord3.interpolateRgb(line.series.color, fnord3.rgb('#d8d8d8'))(0.8).toString();
      } );

      self.graph.update();

    }, false );

    l.element.addEventListener( 'mouseout', function(e) {

      self.legend.lines.forEach( function(line) {
        if (colorSafe[line.series.name]) {
          line.series.color = colorSafe[line.series.name];
        }
      } );

      self.graph.update();

    }, false );
  };

  if (this.legend) {
    this.legend.lines.forEach( function(l) {
      self.addHighlightEvents(l);
    } );
  }

};
FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Graph.Behavior.Series.Toggle');

FnordMetric.rickshaw.Graph.Behavior.Series.Toggle = function(args) {

  this.graph = args.graph;
  this.legend = args.legend;

  var self = this;

  this.addAnchor = function(line) {
    var anchor = document.createElement('a');
    anchor.innerHTML = '&#10004;';
    anchor.classList.add('action');
    line.element.insertBefore(anchor, line.element.firstChild);

    anchor.onclick = function(e) {
      if (line.series.disabled) {
        line.series.enable();
        line.element.classList.remove('disabled');
      } else {
        line.series.disable();
        line.element.classList.add('disabled');
      }
    }

                var label = line.element.getElementsByTagName('span')[0];
                label.onclick = function(e){

                        var disableAllOtherLines = line.series.disabled;
                        if ( ! disableAllOtherLines ) {
                                for ( var i = 0; i < self.legend.lines.length; i++ ) {
                                        var l = self.legend.lines[i];
                                        if ( line.series === l.series ) {
                                                // noop
                                        } else if ( l.series.disabled ) {
                                                // noop
                                        } else {
                                                disableAllOtherLines = true;
                                                break;
                                        }
                                }
                        }

                        // show all or none
                        if ( disableAllOtherLines ) {

                                // these must happen first or else we try ( and probably fail ) to make a no line graph
                                line.series.enable();
                                line.element.classList.remove('disabled');

                                self.legend.lines.forEach(function(l){
                                        if ( line.series === l.series ) {
                                                // noop
                                        } else {
                                                l.series.disable();
                                                l.element.classList.add('disabled');
                                        }
                                });

                        } else {

                                self.legend.lines.forEach(function(l){
                                        l.series.enable();
                                        l.element.classList.remove('disabled');
                                });

                        }

                };

  };

  if (this.legend) {
    this.legend.lines.forEach( function(l) {
      self.addAnchor(l);
    } );
  }

  this._addBehavior = function() {

    this.graph.series.forEach( function(s) {

      s.disable = function() {

        if (self.graph.series.length <= 1) {
          throw('only one series left');
        }

        s.disabled = true;
        self.graph.update();
      };

      s.enable = function() {
        s.disabled = false;
        self.graph.update();
      };
    } );
  };
  this._addBehavior();

  this.updateBehaviour = function () { this._addBehavior() };

};
FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Graph.HoverDetail');

FnordMetric.rickshaw.Graph.HoverDetail = FnordMetric.rickshaw.Class.create({

  initialize: function(args) {

    var graph = this.graph = args.graph;

    this.xFormatter = args.xFormatter || function(x) {
      return FnordMetric.util.dateFormat(x);
    };

    this.yFormatter = args.yFormatter || function(y) {
      return y.toFixed(2);
    };

    var element = this.element = document.createElement('div');
    element.className = 'detail';

    if(args.no_detail){
      element.className = 'detail no_detail';
    }

    this.visible = true;
    graph.element.appendChild(element);

    this.lastEvent = null;
    this._addListeners();

    this.onShow = args.onShow;
    this.onHide = args.onHide;
    this.onRender = args.onRender;

    this.formatter = args.formatter || this.formatter;
  },

  formatter: function(series, x, y, formattedX, formattedY) {
    return series.name + ':&nbsp;' + formattedY;
  },

  update: function(e) {

    e = e || this.lastEvent;
    if (!e) return;
    this.lastEvent = e;

    if (e.target.nodeName != 'path' && e.target.nodeName != 'svg') return;

    var graph = this.graph;

    var eventX = e.offsetX || e.layerX;
    var eventY = e.offsetY || e.layerY;

    var domainX = graph.x.invert(eventX);
    var stackedData = graph.stackedData;

    var topSeriesData = stackedData.slice(-1).shift();

    var domainIndexScale = fnord3.scale.linear()
      .domain([topSeriesData[0].x, topSeriesData.slice(-1).shift().x])
      .range([0, topSeriesData.length]);

    var approximateIndex = Math.floor(domainIndexScale(domainX));
    var dataIndex = approximateIndex || 0;

    for (var i = approximateIndex; i < stackedData[0].length - 1;) {

      if (!stackedData[0][i] || !stackedData[0][i + 1]) {
        break;
      }

      if (stackedData[0][i].x <= domainX && stackedData[0][i + 1].x > domainX) {
        dataIndex = i;
        break;
      }
      if (stackedData[0][i + 1] < domainX) { i++ } else { i-- }
    }

    var domainX = stackedData[0][dataIndex].x;
    var formattedXValue = this.xFormatter(domainX);
    var graphX = graph.x(domainX);
    var order = 0;

    var detail = graph.series.active()
      .map( function(s) { return { order: order++, series: s, name: s.name, value: s.stack[dataIndex] } } );

    var activeItem;

    var sortFn = function(a, b) {
      return (a.value.y0 + a.value.y) - (b.value.y0 + b.value.y);
    };

    var domainMouseY = graph.y.magnitude.invert(graph.element.offsetHeight - eventY);

    detail.sort(sortFn).forEach( function(d) {

      d.formattedYValue = (this.yFormatter.constructor == Array) ?
        this.yFormatter[detail.indexOf(d)](d.value.y) :
        this.yFormatter(d.value.y);

      d.graphX = graphX;
      d.graphY = graph.y(d.value.y0 + d.value.y);

      if (domainMouseY > d.value.y0 && domainMouseY < d.value.y0 + d.value.y && !activeItem) {
        activeItem = d;
        d.active = true;
      }

    }, this );

    this.element.innerHTML = '';
    this.element.style.left = graph.x(domainX) + 'px';

    if (this.visible) {
      this.render( {
        detail: detail,
        domainX: domainX,
        formattedXValue: formattedXValue,
        mouseX: eventX,
        mouseY: eventY
      } );
    }
  },

  hide: function() {
    this.visible = false;
    this.element.classList.add('inactive');

    if (typeof this.onHide == 'function') {
      this.onHide();
    }
  },

  show: function() {
    this.visible = true;
    this.element.classList.remove('inactive');

    if (typeof this.onShow == 'function') {
      this.onShow();
    }
  },

  render: function(args) {

    var detail = args.detail;
    var domainX = args.domainX;

    var mouseX = args.mouseX;
    var mouseY = args.mouseY;

    var formattedXValue = args.formattedXValue;

    var xLabel = document.createElement('div');
    xLabel.className = 'x_label';
    xLabel.innerHTML = formattedXValue;
    this.element.appendChild(xLabel);

    detail.forEach( function(d) {

      var item = document.createElement('div');
      item.className = 'item';
      item.innerHTML = this.formatter(d.series, domainX, d.value.y, formattedXValue, d.formattedYValue);
      item.style.top = this.graph.y(d.value.y0 + d.value.y) + 'px';

      this.element.appendChild(item);

      var dot = document.createElement('div');
      dot.className = 'dot';
      dot.style.top = item.style.top;
      dot.style.borderColor = d.series.color;

      this.element.appendChild(dot);

      if (d.active) {
        item.className = 'item active';
        dot.className = 'dot active';
      }

    }, this );

    this.show();

    if (typeof this.onRender == 'function') {
      this.onRender(args);
    }
  },

  _addListeners: function() {

    this.graph.element.addEventListener(
      'mousemove',
      function(e) {
        this.visible = true;
        this.update(e)
      }.bind(this),
      false
    );

    this.graph.onUpdate( function() { this.update() }.bind(this) );

    this.graph.element.addEventListener(
      'mouseout',
      function(e) {
        if (e.relatedTarget && !(e.relatedTarget.compareDocumentPosition(this.graph.element) & Node.DOCUMENT_POSITION_CONTAINS)) {
          this.hide();
        }
       }.bind(this),
      false
    );
  }
});

FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Graph.Legend');

FnordMetric.rickshaw.Graph.Legend = function(args) {

  var element = this.element = args.element;
  var graph = this.graph = args.graph;

  var self = this;

  element.classList.add('fnordmetric_legend');

  var list = this.list = document.createElement('ul');
  element.appendChild(list);

  var series = graph.series
    .map( function(s) { return s } )
    .reverse();

  this.lines = [];

  this.addLine = function (series) {
    var line = document.createElement('li');
    line.className = 'line';

    var swatch = document.createElement('div');
    swatch.className = 'swatch';
    swatch.style.backgroundColor = series.color;

    line.appendChild(swatch);

    var label = document.createElement('span');
    label.className = 'label';
    label.innerHTML = series.name;

    line.appendChild(label);
    list.appendChild(line);

    line.series = series;

    if (series.noLegend) {
      line.style.display = 'none';
    }

    var _line = { element: line, series: series };
    if (self.shelving) {
      self.shelving.addAnchor(_line);
      self.shelving.updateBehaviour();
    }
    if (self.highlighter) {
      self.highlighter.addHighlightEvents(_line);
    }
    self.lines.push(_line);
  };

  series.forEach( function(s) {
    self.addLine(s);
  } );

  graph.onUpdate( function() {

  } );
};
FnordMetric.rickshaw.namespace("FnordMetric.rickshaw.Graph.Renderer");

FnordMetric.rickshaw.Graph.Renderer = FnordMetric.rickshaw.Class.create( {

  initialize: function(args) {
    this.graph = args.graph;
    this.tension = args.tension || this.tension;
    this.graph.unstacker = this.graph.unstacker || new FnordMetric.rickshaw.Graph.Unstacker( { graph: this.graph } );
    this.configure(args);
  },

  seriesPathFactory: function() {
    //implement in subclass
  },

  seriesStrokeFactory: function() {
    // implement in subclass
  },

  defaults: function() {
    return {
      tension: 0.8,
      strokeWidth: 2,
      unstack: true,
      padding: { top: 0.01, right: 0, bottom: 0.01, left: 0 },
      stroke: false,
      fill: false
    };
  },

  domain: function() {

    var values = [];
    var stackedData = this.graph.stackedData || this.graph.stackData();

    var topSeriesData = this.unstack ? stackedData : [ stackedData.slice(-1).shift() ];

    topSeriesData.forEach( function(series) {
      series.forEach( function(d) {
        values.push( d.y + d.y0 );
      } );
    } );

    var xMin = stackedData[0][0].x;
    var xMax = stackedData[0][ stackedData[0].length - 1 ].x;

    xMin -= (xMax - xMin) * this.padding.left;
    xMax += (xMax - xMin) * this.padding.right;

    var yMin = this.graph.min === 'auto' ? fnord3.min( values ) : this.graph.min || 0;
    var yMax = this.graph.max || fnord3.max( values );

    if (this.graph.min === 'auto' || yMin < 0) {
      yMin -= (yMax - yMin) * this.padding.bottom;
    }

    if (this.graph.max === undefined) {
      yMax += (yMax - yMin) * this.padding.top;
    }

    return { x: [xMin, xMax], y: [yMin, yMax] };
  },

  render: function() {

    var graph = this.graph;

    graph.vis.selectAll('*').remove();

    var nodes = graph.vis.selectAll("path")
      .data(this.graph.stackedData)
      .enter().append("svg:path")
      .attr("d", this.seriesPathFactory());

    var i = 0;
    graph.series.forEach( function(series) {
      if (series.disabled) return;
      series.path = nodes[0][i++];
      this._styleSeries(series);
    }, this );
  },

  _styleSeries: function(series, fm_opts) {

    var fill = this.fill ? series.color : 'none';
    var stroke = this.stroke ? series.color : 'none';

    series.path.setAttribute('fill', fill);
    series.path.setAttribute('stroke', stroke);
    if (fm_opts){
      series.path.setAttribute('stroke-width', fm_opts.stroke_width);
    } else {
      series.path.setAttribute('stroke-width', this.strokeWidth);
    }
    series.path.setAttribute('class', series.className);
  },

  configure: function(args) {

    args = args || {};

    FnordMetric.rickshaw.keys(this.defaults()).forEach( function(key) {

      if (!args.hasOwnProperty(key)) {
        this[key] = this[key] || this.graph[key] || this.defaults()[key];
        return;
      }

      if (typeof this.defaults()[key] == 'object') {

        FnordMetric.rickshaw.keys(this.defaults()[key]).forEach( function(k) {

          this[key][k] =
            args[key][k] !== undefined ? args[key][k] :
            this[key][k] !== undefined ? this[key][k] :
            this.defaults()[key][k];
        }, this );

      } else {
        this[key] =
          args[key] !== undefined ? args[key] :
          this[key] !== undefined ? this[key] :
          this.graph[key] !== undefined ? this.graph[key] :
          this.defaults()[key];
      }

    }, this );
  },

  setStrokeWidth: function(strokeWidth) {
    if (strokeWidth !== undefined) {
      this.strokeWidth = strokeWidth;
    }
  },

  setTension: function(tension) {
    if (tension !== undefined) {
      this.tension = tension;
    }
  }
} );

FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Graph.Renderer.Line');

FnordMetric.rickshaw.Graph.Renderer.Line = FnordMetric.rickshaw.Class.create( FnordMetric.rickshaw.Graph.Renderer, {

  name: 'line',

  defaults: function($super) {

    return FnordMetric.rickshaw.extend( $super(), {
      unstack: true,
      fill: false,
      stroke: true
    } );
  },

  seriesPathFactory: function() {

    var graph = this.graph;

    return fnord3.svg.line()
      .x( function(d) { return graph.x(d.x) } )
      .y( function(d) { return graph.y(d.y) } )
      .interpolate(this.graph.interpolation).tension(this.tension);
  },


  render: function() {

    if(this.graph.stackedData[0].length < 42){
      var fm_opts = { stroke_width: 3, draw_points: true };
    } else if(this.graph.stackedData[0].length < 99){
      var fm_opts = { stroke_width: 2, draw_points: false };
    } else {
      var fm_opts = { stroke_width: 1, draw_points: false };
    }

    var graph = this.graph;

    graph.vis.selectAll('*').remove();

    var nodes = graph.vis.selectAll("path")
      .data(this.graph.stackedData)
      .enter().append("svg:path")
      .attr("d", this.seriesPathFactory());

    if(fm_opts.draw_points){
    }

    var i = 0;
    graph.series.forEach( function(series) {
      if (series.disabled) return;
      series.path = nodes[0][i++];
      this._styleSeries(series, fm_opts);
    }, this );

  },

} );

FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Graph.Renderer.Stack');

FnordMetric.rickshaw.Graph.Renderer.Stack = FnordMetric.rickshaw.Class.create( FnordMetric.rickshaw.Graph.Renderer, {

  name: 'stack',

  defaults: function($super) {

    return FnordMetric.rickshaw.extend( $super(), {
      fill: true,
      stroke: false,
      unstack: false,
    } );
  },

  seriesPathFactory: function() {

    var graph = this.graph;

    return fnord3.svg.area()
      .x( function(d) { return graph.x(d.x) } )
      .y0( function(d) { return graph.y(d.y0) } )
      .y1( function(d) { return graph.y(d.y + d.y0) } )
      .interpolate(this.graph.interpolation).tension(this.tension);
  },

  render: function() {
    var graph = this.graph;

    graph.vis.selectAll('*').remove();

    var nodes = graph.vis.selectAll("path")
      .data(this.graph.stackedData)
      .enter().append("svg:path")
      .attr("d", this.seriesPathFactory());

    var i = 0;
    graph.series.forEach( function(series) {
      if (series.disabled) return;
      series.path = nodes[0][i++];
      this._styleSeries(series);
    }, this );
  },

  _styleSeries: function(series, fm_opts) {

    var fill = this.fill ? series.color : 'none';
    var stroke = this.stroke ? series.color : 'none';

    series.path.setAttribute('fill', fnord3.interpolateRgb(fill, 'white')(0.125))
    series.path.setAttribute('stroke', stroke);
    if (fm_opts){
      series.path.setAttribute('stroke-width', fm_opts.stroke_width);
    } else {
      series.path.setAttribute('stroke-width', this.strokeWidth);
    }
    series.path.setAttribute('class', series.className);
  },


} );

FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Graph.Renderer.Bar');

FnordMetric.rickshaw.Graph.Renderer.Bar = FnordMetric.rickshaw.Class.create( FnordMetric.rickshaw.Graph.Renderer, {

  name: 'bar',

  defaults: function($super) {

    var defaults = FnordMetric.rickshaw.extend( $super(), {
      gapSize: 0.10,
      unstack: false,
    } );

    delete defaults.tension;
    return defaults;
  },

  initialize: function($super, args) {
    args = args || {};
    this.gapSize = args.gapSize || this.gapSize;
    this.xPadding = args.xPadding || 50;
    $super(args);
  },

  domain: function($super) {

    var domain = $super();

    var frequentInterval = this._frequentInterval();
    domain.x[1] += parseInt(frequentInterval.magnitude);

    return domain;
  },

  barWidth: function() {
    var stackedData = this.graph.stackedData || this.graph.stackData();
    var data = stackedData.slice(-1).shift();

    var frequentInterval = this._frequentInterval();
    var barWidth = this.graph.x(data[0].x + frequentInterval.magnitude * (1 - this.gapSize));

    return ((this.graph.width - (this.xPadding * 2)) / data.length);
  },

  render: function() {

    var graph = this.graph;

    graph.vis.selectAll('*').remove();

    var barWidth = this.barWidth();
    var barXOffset = 0;

    var activeSeriesCount = graph.series.filter( function(s) { return !s.disabled; } ).length;
    var seriesBarWidth = this.unstack ? barWidth / activeSeriesCount : barWidth;

    graph.series.forEach( function(series) {

      if (series.disabled) return;

      var xpad = this.xPadding;

      var seriesBarDrawWidth = Math.min(60,
        parseInt(seriesBarWidth * (1 - this.gapSize)));

      if(parseInt(seriesBarWidth) == seriesBarDrawWidth){
        seriesBarDrawWidth -= 1;
      }

      var seriesBarDrawPadding = (seriesBarWidth - seriesBarDrawWidth) / 2;

      var nodes = graph.vis.selectAll("path")
        .data(series.stack)
        .enter().append("svg:rect")
        .attr("x", function(d) { return xpad + (d.x * seriesBarWidth) + seriesBarDrawPadding })
        .attr("y", function(d) { return graph.y(d.y0 + d.y) })
        .attr("width", seriesBarDrawWidth)
        .attr("stroke", "#000")
        .attr("stroke-width", "1px")
        .attr("stroke-opacity", "0.6")
        .attr("height", function(d) { return graph.y.magnitude(d.y) });


      var sdata = series.stack;

      for(var ind=0; ind < sdata.length; ind++){
        $(graph.element).append(
          $("<div>")
            .css("position", "absolute")
            .css("color", "#666")
            .css("width", seriesBarWidth)
            .css("textAlign", "center")
            .css("marginTop", "5px")
            .css("marginLeft", xpad + (sdata[ind].x * seriesBarWidth))
            .css("y", graph.height)
            .html(sdata[ind].label)
        );
        //
      }

      Array.prototype.forEach.call(nodes[0], function(n) {
        n.setAttribute('fill', series.color);
      } );


      var total = $('.ui_numbers .samples').data('value');
      $('.widget_histogram_bars .tooltip').remove();
      $('.widget_histogram_bars rect').each(function(hist_i) {
        var percentage = Math.round(sdata[hist_i].y * 1000 / total) / 10;
        var left = parseInt($(this).offset().left);
        var top = parseInt($(this).offset().top) - 23;
        var tooltip = '<div class="tooltip" data-hist-id="' + hist_i
          + '" style="left:' + left + 'px; top: ' + top + 'px">'
          + sdata[hist_i].y + ' (' + percentage + '%)' + '</div>';
        $(this).parents('.widget_histogram_bars:first').append(tooltip);
        $(this).attr('data-id', hist_i);
      });

      $('.widget_histogram_bars rect').hover(function() {
        $('.widget_histogram_bars .tooltip[data-hist-id=' +
          $(this).attr('data-id') + ']').show();
      }, function() {
        $('.widget_histogram_bars .tooltip[data-hist-id=' +
          $(this).attr('data-id') + ']').fadeOut();
      });


    }, this );
  },

  _frequentInterval: function() {

    var stackedData = this.graph.stackedData || this.graph.stackData();
    var data = stackedData.slice(-1).shift();

    var intervalCounts = {};

    for (var i = 0; i < data.length - 1; i++) {
      var interval = data[i + 1].x - data[i].x;
      intervalCounts[interval] = intervalCounts[interval] || 0;
      intervalCounts[interval]++;
    }

    var frequentInterval = { count: 0 };

    FnordMetric.rickshaw.keys(intervalCounts).forEach( function(i) {
      if (frequentInterval.count < intervalCounts[i]) {

        frequentInterval = {
          count: intervalCounts[i],
          magnitude: i
        };
      }
    } );

    this._frequentInterval = function() { return frequentInterval };

    return frequentInterval;
  }
} );

FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Graph.Renderer.Area');

FnordMetric.rickshaw.Graph.Renderer.Area = FnordMetric.rickshaw.Class.create( FnordMetric.rickshaw.Graph.Renderer, {

  name: 'area',

  defaults: function($super) {

    return FnordMetric.rickshaw.extend( $super(), {
      unstack: false,
      fill: false,
      stroke: false
    } );
  },

  seriesPathFactory: function() {

    var graph = this.graph;

    return fnord3.svg.area()
      .x( function(d) { return graph.x(d.x) } )
      .y0( function(d) { return graph.y(d.y0) } )
      .y1( function(d) { return graph.y(d.y + d.y0) } )
      .interpolate(graph.interpolation).tension(this.tension);
  },

  seriesStrokeFactory: function() {

    var graph = this.graph;

    return fnord3.svg.line()
      .x( function(d) { return graph.x(d.x) } )
      .y( function(d) { return graph.y(d.y + d.y0) } )
      .interpolate(graph.interpolation).tension(this.tension);
  },

  render: function() {

    var graph = this.graph;

    graph.vis.selectAll('*').remove();

    if(this.graph.stackedData[0].length < 42){
      var fm_opts = { stroke_width: 3 };
    } else {
      var fm_opts = { stroke_width: 1 };
    }

    var nodes = graph.vis.selectAll("path")
      .data(this.graph.stackedData)
      .enter().insert("svg:g", 'g');

    nodes.append("svg:path")
      .attr("d", this.seriesPathFactory())
      .attr("class", 'area');

    if (this.stroke) {
      nodes.append("svg:path")
        .attr("d", this.seriesStrokeFactory())
        .attr("class", 'line');
    }

    var i = 0;
    graph.series.forEach( function(series) {
      if (series.disabled) return;
      series.path = nodes[0][i++];
      this._styleSeries(series, fm_opts);
    }, this );
  },

  _styleSeries: function(series, fm_opts) {

    if (!series.path) return;

    fnord3.select(series.path).select('.area')
      .attr('opacity', '0.65')
      .attr('fill', series.color);

    fnord3.select(series.path).select('.line')
      .attr('fill', 'none')
      .attr('stroke', fnord3.interpolateRgb(series.color, 'white')(0.125))
      .attr('stroke-width', fm_opts.stroke_width);

    if (series.className) {
      series.path.setAttribute('class', series.className);
    }
  }
} );

FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Graph.Unstacker');

FnordMetric.rickshaw.Graph.Unstacker = function(args) {

  this.graph = args.graph;
  var self = this;

  this.graph.stackData.hooks.after.push( {
    name: 'unstacker',
    f: function(data) {

      if (!self.graph.renderer.unstack) return data;

      data.forEach( function(seriesData) {
        seriesData.forEach( function(d) {
          d.y0 = 0;
        } );
      } );

      return data;
    }
  } );
};

FnordMetric.rickshaw.namespace('FnordMetric.rickshaw.Series');

FnordMetric.rickshaw.Series = FnordMetric.rickshaw.Class.create( Array, {

  initialize: function (data, palette, options) {

    options = options || {}

    this.palette = new FnordMetric.rickshaw.Color.Palette(palette);

    this.timeBase = typeof(options.timeBase) === 'undefined' ?
      Math.floor(new Date().getTime() / 1000) :
      options.timeBase;

    if (data && (typeof(data) == "object") && (data instanceof Array)) {
      data.forEach( function(item) { this.addItem(item) }, this );
    }
  },

  addItem: function(item) {

    if (typeof(item.name) === 'undefined') {
      throw('addItem() needs a name');
    }

    item.color = (item.color || this.palette.color(item.name));
    item.data = (item.data || []);

    // backfill, if necessary
    if ((item.data.length == 0) && this.length && (this.getIndex() > 0)) {
      this[0].data.forEach( function(plot) {
        item.data.push({ x: plot.x, y: 0 });
      } );
    } else if (item.data.length == 0) {
      item.data.push({ x: this.timeBase - (this.timeInterval || 0), y: 0 });
    }

    this.push(item);

    if (this.legend) {
      this.legend.addLine(this.itemByName(item.name));
    }
  },

  addData: function(data) {

    var index = this.getIndex();

    FnordMetric.rickshaw.keys(data).forEach( function(name) {
      if (! this.itemByName(name)) {
        this.addItem({ name: name });
      }
    }, this );

    this.forEach( function(item) {
      item.data.push({
        x: (index * this.timeInterval || 1) + this.timeBase,
        y: (data[item.name] || 0)
      });
    }, this );
  },

  getIndex: function () {
    return (this[0] && this[0].data && this[0].data.length) ? this[0].data.length : 0;
  },

  itemByName: function(name) {

    for (var i = 0; i < this.length; i++) {
      if (this[i].name == name)
        return this[i];
    }
  },

  setTimeInterval: function(iv) {
    this.timeInterval = iv / 1000;
  },

  setTimeBase: function (t) {
    this.timeBase = t;
  },

  dump: function() {

    var data = {
      timeBase: this.timeBase,
      timeInterval: this.timeInterval,
      items: [],
    };

    this.forEach( function(item) {

      var newItem = {
        color: item.color,
        name: item.name,
        data: []
      };

      item.data.forEach( function(plot) {
        newItem.data.push({ x: plot.x, y: plot.y });
      } );

      data.items.push(newItem);
    } );

    return data;
  },

  load: function(data) {

    if (data.timeInterval) {
      this.timeInterval = data.timeInterval;
    }

    if (data.timeBase) {
      this.timeBase = data.timeBase;
    }

    if (data.items) {
      data.items.forEach( function(item) {
        this.push(item);
        if (this.legend) {
          this.legend.addLine(this.itemByName(item.name));
        }

      }, this );
    }
  }
} );
// FnordMetric UI
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

if (typeof FnordMetric == 'undefined')
  FnordMetric = {};

if (typeof FnordMetric.util == 'undefined')
  FnordMetric.util = {};

FnordMetric.util.decPrint = function(val){
  return (val < 10 ? '0'+val : val);
}

FnordMetric.util.formatTimeRange = function(range){
  if (range < 60){
    return parseInt(range) + ' sec';
  } else if(range<3600){
    return parseInt(range/60) + ' min';
  } else if(range==3600){
    return '1 hour';
  } else if(range<(3600*24)){
    return parseInt(range/3600) + ' hours';
  } else if(range==(3600*24)){
    return '1 day';
  } else {
    return parseInt(range/(3600*24)) + ' days';
  }
}

FnordMetric.util.formatValue = function(value, round_to){
  if (typeof round_to == 'undefined') {
    round_to = 1;
  }

  if (value < 10) {
    return value.toFixed(round_to);
  } else if (value < 100) {
    return value.toFixed(round_to);
  } else if (value > 1000000) {
    return (value / 1000000.0).toFixed(round_to) + "m";
  } else if (value > 1000) {
    return (value / 1000.0).toFixed(round_to) + "k";
  } else {
    return value.toFixed(round_to);
  }
}

FnordMetric.util.formatTimeValue = function(value){
  if (value < 60){
    return parseFloat(value).toFixed(1) + 's';
  } else if(value<3600){
    return parseFloat(value/60).toFixed(1) + 'm';
  } else if(value<(3600*24)){
    return parseFloat(value/3600).toFixed(1) + 'h';
  } else {
    return parseFloat(value/(3600*24)).toFixed(1) + 'd';
  }
}

FnordMetric.util.formatPercentValue = function(value){
  value = parseFloat(value);
  if(value < 10){
    return value.toFixed(2) + '%';
  } else if(value < 100){
    return value.toFixed(1) + '%';
  } else {
    return '100%';
  }
}

FnordMetric.util.dateFormat = function(timestamp){
  var t = new Date(timestamp*1000);
  return FnordMetric.util.decPrint(t.getFullYear())  + "/" +
         FnordMetric.util.decPrint((t.getMonth()+1)) + "/" + 
         FnordMetric.util.decPrint(t.getDate())      + " " + 
         FnordMetric.util.decPrint(t.getHours())     + ":" +
         FnordMetric.util.decPrint(t.getMinutes())   + " ";
}

FnordMetric.util.dateFormatWithRange = function(timestamp, range){
  var t = new Date(timestamp*1000);

  if (range >= (3600 * 24))
    return FnordMetric.util.decPrint((t.getMonth()+1)) + "/" + 
           FnordMetric.util.decPrint(t.getDate())      + " " + 
           FnordMetric.util.decPrint(t.getHours())     + ":" +
           FnordMetric.util.decPrint(t.getMinutes());
  else if (range >= 900)
    return FnordMetric.util.decPrint(t.getHours())     + ":" +
           FnordMetric.util.decPrint(t.getMinutes());
  else
    return FnordMetric.util.decPrint(t.getHours())     + ":" +
           FnordMetric.util.decPrint(t.getMinutes())   + ":" +
           FnordMetric.util.decPrint(t.getSeconds());
}

FnordMetric.util.generateUUID = function () {
  return Math.floor((1 + Math.random()) * 0x1000000).toString(16);
}

FnordMetric.util.parseTime = function(str) {
  var res,
      now = parseInt(((new Date()).getTime() / 1000), 10),
      str = str.toLowerCase();

  if (str == "now") {
    return now;
  } else if ((res = str.match( /^([0-9]+)$/)) != null) {
    return parseInt(res[1], 10);
  } else if ((res = str.match( /^-([0-9]+)$/)) != null) {
    return (now - parseInt(res[1], 10));
  } else if ((res = str.match( /^-([0-9]+)s(ec(ond)?(s?))?$/)) != null) {
    return (now - parseInt(res[1], 10));
  } else if ((res = str.match( /^-([0-9]+(?:\.[0-9]+)?)m(in(ute)?(s?))?$/)) != null) {
    return parseInt(now - (parseFloat(res[1]) * 60), 10);
  } else if ((res = str.match( /^-([0-9]+(?:\.[0-9]+)?)h(our(s?))?$/)) != null) {
    return parseInt(now - (parseFloat(res[1]) * 3600), 10);
  } else if ((res = str.match( /^-([0-9]+(?:\.[0-9]+)?)d(ay(s?))?$/)) != null) {
    return parseInt(now - (parseFloat(res[1]) * 86400), 10);
  } else {
    console.log("[FnordMetric] invalid time specifiation: " + str);
  }
}

FnordMetric.util.zeroFill = function(obj, since, until) {
  var ticks = []

  for (key in obj) {
    for (tick in obj[key]) {
      var t = parseInt(tick, 10);

      if (ticks.indexOf(t) == -1)
        ticks.push(t);
     }
  }

  ticks.sort();

  for (key in obj) {
    var m = /-([0-9]+)$/.exec(key);
    var tl = -1;
    var ts = -1;

    if (m != null)
      ts = parseInt(m[1], 10);

    if (typeof since != "undefined")
      tl = since + ts;

    var lv = 0;

    for (ind in ticks) {
      if (ts > 0 && tl > 0) {
        while (ticks[ind] - tl > ts) {
          tl += ts;
          obj[key][tl] = 0;
        }
      }

      tl = ticks[ind];

      if (typeof obj[key][ticks[ind]] == 'undefined')
        obj[key][ticks[ind]] = lv;
      else
        lv = obj[key][ticks[ind]];
    }

    if (typeof until != "undefined") {
      while (tl < until - ts) {
        tl += ts;
        obj[key][tl] = 0;
      }
    }

  }

  return obj;
}
// FnordMetric UI
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

if (typeof FnordMetric == 'undefined')
  FnordMetric = {};

if (typeof FnordMetric.widgets == 'undefined')
  FnordMetric.widgets = {};

FnordMetric.widgets.timeseries = function(elem){
  var graph, gauges, colors, gconfig, legend, hoverDetail, shelving,
      highlighter, refresh_timer, series, height, display_legend,
      widget_key;

  var default_colors = ["#db843d", "#3d96ae", "#80699b", "#89a54e",
                         "#aa4643", "#4572a7"]

  function init() {
    widget_key = elem.attr("data-widget-key");

    if (elem.attr('data-legend') == "off")
      display_legend = false;
    else
      display_legend = true;

    renderLayout();

    if (!elem.attr('data-gauges'))
      return console.log("[FnordMetric] element is missing the data-gauges attribute");

    if (!elem.attr('data-since'))
      return console.log("[FnordMetric] element is missing the data-since attribute");

    if (!elem.attr('data-until'))
      return console.log("[FnordMetric] element is missing the data-since attribute");

    gauges = elem.attr("data-gauges").split(",");

    if (elem.attr('data-colors'))
      colors = elem.attr("data-colors").split(",");
    else
      colors = [];

    for (n = 0; n < gauges.length; n++)
      if (!colors[n]) colors[n] = default_colors.pop();

    gconfig = {
      element: $('.fnordmetric_container', elem)[0],
      padding: { top: 0.1, bottom: 0 },
      stroke: true
    };


    if (elem.attr('data-y-max')) {
      gconfig.max = parseFloat(elem.attr('data-y-max'));
    } else {
      delete gconfig.max;
    }

    if (elem.attr('data-cardinal') == "on") {
      gconfig.interpolation = 'cardinal';
    } else {
      gconfig.interpolation = 'linear';
    }

    if (elem.attr('data-chart-style') == 'area') {
      gconfig.renderer = 'area';
      gconfig.offset = 'zero';
    }

    else if (elem.attr('data-chart-style') == 'flow') {
      gconfig.renderer = 'stack';
      gconfig.offset = 'silhouette';
    }

    else {
      gconfig.renderer = 'line';
      gconfig.offset = 'zero';
    }

    if (elem.attr('data-height'))
      height = parseInt(elem.attr('data-height'), 10);
    else
      height = 240;

    requestDataAsync();

    if (refresh_interval = elem.attr('data-autoupdate'))
      refresh_interval = parseFloat(refresh_interval);

    if (refresh_interval)
      refresh_timer = window.setInterval(
        requestDataAsync, refresh_interval * 1000);
  }

  function renderLayout() {
    if (display_legend)
      $(elem).append(
        $('<div></div>')
          .addClass('fnordmetric_container_legend')
          .css({
            margin: '10px 30px 0 30px',
          })
      );

    $(elem).append(
      $('<div></div>')
        .addClass('fnordmetric_container')
        .css({
          height: height,
          margin: '0 23px 25px 23px',
        })
    );
  }

  function renderChart() {
    gconfig.width = elem.width() - 50;
    gconfig.height = height;

    $(gconfig.element).html("");

    if (display_legend)
      $(".fnordmetric_legend", elem).html("");

    graph = new FnordMetric.rickshaw.Graph(gconfig);

    if (display_legend)
      legend = new FnordMetric.rickshaw.Graph.Legend({
        graph: graph,
        element: $('.fnordmetric_container_legend', elem)[0]
      });

    hoverDetail = new FnordMetric.rickshaw.Graph.HoverDetail( {
      graph: graph
    });

    shelving = new FnordMetric.rickshaw.Graph.Behavior.Series.Toggle({
      graph: graph,
      legend: legend
    });

    highlighter = new FnordMetric.rickshaw.Graph.Behavior.Series.Highlight({
      graph: graph,
      legend: legend
    });

    new FnordMetric.rickshaw.Graph.Axis.Time({
      graph: graph,
    }).render();

    new FnordMetric.rickshaw.Graph.Axis.Y({
      graph: graph,
    }).render();

    if(!gconfig.renderer){
      gconfig.renderer = "line";
    }

    graph.configure(gconfig);
    graph.render();
  }

  function resize() {
    requestDataAsync();
  }

  function send(evt) {
    var dgauges = evt.gauges;
    gconfig.series = [];

    for(var ind = 0; ind < dgauges.length; ind++){

      gconfig.series.push({
        name: dgauges[ind].title,
        color: colors[ind],
        data: []
      });

      for(_time in dgauges[ind].vals){
        gconfig.series[ind].data.push(
          { x: parseInt(_time), y: parseInt(dgauges[ind].vals[_time] || 0) }
        );
      }

    }

    renderChart();
  }

  function requestDataAsync() {
    var since = elem.attr("data-since"),
        until = elem.attr("data-until");

    FnordMetric.values_in(gauges, since, until, function(){
      var gauges = Object.keys(this)
      gconfig.series = [];

      for (ind in gauges) {
        var gauge = gauges[ind];

        gconfig.series.push({
          name: gauge,
          color: colors[ind],
          data: []
        });

        for(_time in this[gauge]){
          gconfig.series[ind].data.push(
            { x: parseInt(_time), y: parseInt(this[gauge][_time] || 0) }
          );
        }
      }

      renderChart();
    });
  }

  function destroy() {
    if (refresh_timer)
      window.clearInterval(refresh_timer);
  }


  return {
    init: init,
    send: send,
    destroy: destroy,
    resize: resize
  };

};
// FnordMetric UI
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

if (typeof FnordMetric == 'undefined')
  FnordMetric = {};

if (typeof FnordMetric.widgets == 'undefined')
  FnordMetric.widgets = {};

FnordMetric.widgets.counter = function(elem){
  var gauge, at, scale_by, refresh_timer, refresh_interval;
  var widget_key = elem.attr("data-widget-key");

  function init() {
    gauge = elem.attr('data-gauge');

    if (!gauge)
      return console.log("[FnordMetric] element is missing the data-gauge attribute");

    at = elem.attr('data-at');
    if (!at) at = "now";

    if (scale_by = elem.attr('data-scale-by'))
      scale_by = parseFloat(elem.attr('data-scale-by'));
    else
      scale_by = 1;

    requestDataAsync();

    if (refresh_interval = elem.attr('data-autoupdate'))
      refresh_interval = parseFloat(refresh_interval);

    if (refresh_interval)
      refresh_timer = window.setInterval(
        requestDataAsync, refresh_interval * 1000);
  }

  function send(evt) {
    elem.attr('data-value', evt.value);
    updateDisplay();
  }

  function requestDataAsync() {
    FnordMetric.value_at(gauge, (at + ""), function(){
      elem.attr('data-value', this.value);
      updateDisplay();
    });
  }

  function destroy() {
    if (refresh_timer)
      window.clearInterval(refresh_timer);
  }

  function updateDisplay() {
    var target_val = parseFloat(elem.attr('data-value'));
    if(!target_val){ target_val=0; }

    var current_val = parseFloat(elem.attr('data-current'));
    if(!current_val){ current_val=0; }

    var diff = (target_val-current_val) / 4;

    if((diff > 0) && (diff < 1)){ diff=1; }
    if((diff < 0) && (diff > -1)){ diff=-1; }

    if(target_val != current_val){
      var new_val = current_val + diff;

      if((diff > 0) && (new_val > target_val)){ new_val = target_val; }
      if((diff < 0) && (new_val < target_val)){ new_val = target_val; }

      if (elem.attr('data-round-to'))
        var val_txt = FnordMetric.util.formatValue(new_val * scale_by,
          parseInt(elem.attr('data-round-to'), 10));
      else
        var val_txt = FnordMetric.util.formatValue(new_val * scale_by);

      if (elem.attr('data-unit'))
        val_txt += elem.attr('data-unit');

      elem.attr('data-current', new_val);
      elem.html(val_txt);

      (function(){
        window.setTimeout(updateDisplay, 30);
      })();
    }
  }

  return {
    init: init,
    send: send,
    destroy: destroy
  };

};

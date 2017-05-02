/* from https://github.com/que-etc/resize-observer-polyfill
The MIT License (MIT)

Copyright (c) 2016 Denis Rul

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*/
(function (global, factory) {
    typeof exports === 'object' && typeof module !== 'undefined' ? module.exports = factory() :
    typeof define === 'function' && define.amd ? define(factory) :
    (global.ResizeObserver = factory());
}(this, (function () {
'use strict';

/**
 * Exports global object for the current environment.
 */
var global$1 = (function () {
    if (typeof self != 'undefined' && self.Math === Math) {
        return self;
    }

    if (typeof window != 'undefined' && window.Math === Math) {
        return window;
    }

    if (typeof global != 'undefined' && global.Math === Math) {
        return global;
    }

    // eslint-disable-next-line no-new-func
    return Function('return this')();
})();

var classCallCheck = function (instance, Constructor) {
    if (!(instance instanceof Constructor)) {
        throw new TypeError("Cannot call a class as a function");
    }
};

var createClass = function () {
    function defineProperties(target, props) {
        for (var i = 0; i < props.length; i++) {
            var descriptor = props[i];
            descriptor.enumerable = descriptor.enumerable || false;
            descriptor.configurable = true;
            if ("value" in descriptor) descriptor.writable = true;
            Object.defineProperty(target, descriptor.key, descriptor);
        }
    }

    return function (Constructor, protoProps, staticProps) {
        if (protoProps) defineProperties(Constructor.prototype, protoProps);
        if (staticProps) defineProperties(Constructor, staticProps);
        return Constructor;
    };
}();

var inherits = function (subClass, superClass) {
    if (typeof superClass !== "function" && superClass !== null) {
        throw new TypeError("Super expression must either be null or a function, not " + typeof superClass);
    }

    subClass.prototype = Object.create(superClass && superClass.prototype, {
        constructor: {
            value: subClass,
            enumerable: false,
            writable: true,
            configurable: true
        }
    });
    if (superClass) Object.setPrototypeOf ? Object.setPrototypeOf(subClass, superClass) : subClass.__proto__ = superClass;
};

var possibleConstructorReturn = function (self, call) {
    if (!self) {
        throw new ReferenceError("this hasn't been initialised - super() hasn't been called");
    }

    return call && (typeof call === "object" || typeof call === "function") ? call : self;
};

/**
 * A collection of shims that provides minimal
 * support of WeakMap and Map classes.
 *
 * These implementations are not meant to be used outside of
 * ResizeObserver modules as they cover only a limited range
 * of use cases.
 */
/* eslint-disable require-jsdoc */
var hasNativeCollections = typeof global$1.WeakMap === 'function' && typeof global$1.Map === 'function';

var WeakMap = function () {
    if (hasNativeCollections) {
        return global$1.WeakMap;
    }

    function getIndex(arr, key) {
        var result = -1;

        arr.some(function (entry, index) {
            var matches = entry[0] === key;

            if (matches) {
                result = index;
            }

            return matches;
        });

        return result;
    }

    return function () {
        function _class() {
            classCallCheck(this, _class);

            this.__entries__ = [];
        }

        _class.prototype.get = function get(key) {
            var index = getIndex(this.__entries__, key);

            return this.__entries__[index][1];
        };

        _class.prototype.set = function set(key, value) {
            var index = getIndex(this.__entries__, key);

            if (~index) {
                this.__entries__[index][1] = value;
            } else {
                this.__entries__.push([key, value]);
            }
        };

        _class.prototype.delete = function _delete(key) {
            var entries = this.__entries__,
                index = getIndex(entries, key);

            if (~index) {
                entries.splice(index, 1);
            }
        };

        _class.prototype.has = function has(key) {
            return !!~getIndex(this.__entries__, key);
        };

        return _class;
    }();
}();

var Map = function () {
    if (hasNativeCollections) {
        return global$1.Map;
    }

    return function (_WeakMap) {
        inherits(_class2, _WeakMap);

        function _class2() {
            classCallCheck(this, _class2);
            return possibleConstructorReturn(this, _WeakMap.apply(this, arguments));
        }

        _class2.prototype.clear = function clear() {
            this.__entries__.splice(0, this.__entries__.length);
        };

        _class2.prototype.entries = function entries() {
            return this.__entries__.slice();
        };

        _class2.prototype.keys = function keys() {
            return this.__entries__.map(function (entry) {
                return entry[0];
            });
        };

        _class2.prototype.values = function values() {
            return this.__entries__.map(function (entry) {
                return entry[1];
            });
        };

        _class2.prototype.forEach = function forEach(callback) {
            var ctx = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : null;

            for (var _iterator = this.__entries__, _isArray = Array.isArray(_iterator), _i = 0, _iterator = _isArray ? _iterator : _iterator[Symbol.iterator]();;) {
                var _ref;

                if (_isArray) {
                    if (_i >= _iterator.length) break;
                    _ref = _iterator[_i++];
                } else {
                    _i = _iterator.next();
                    if (_i.done) break;
                    _ref = _i.value;
                }

                var entry = _ref;

                callback.call(ctx, entry[1], entry[0]);
            }
        };

        createClass(_class2, [{
            key: 'size',
            get: function get() {
                return this.__entries__.length;
            }
        }]);
        return _class2;
    }(WeakMap);
}();

/**
 * Detects whether window and document objects are available in current environment.
 */
var isBrowser = global$1.window === global$1 && typeof document != 'undefined';

// Placeholder of an empty content rectangle.
var emptyRect = createContentRect(0, 0, 0, 0);

/**
 * Extracts computed styles of provided element.
 *
 * @param {Element} target
 * @returns {CSSStyleDeclaration}
 */
function getStyles(target) {
    return getComputedStyle(target);
}

/**
 * Converts provided string defined in q form of '{{value}}px' to number.
 *
 * @param {String} value
 * @returns {Number}
 */
function pixelsToNumber(value) {
    return parseFloat(value) || 0;
}

/**
 * Extracts borders size from provided styles.
 *
 * @param {CSSStyleDeclaration} styles
 * @param {...String} positions - Borders positions (top, right, ...)
 * @returns {Number}
 */
function getBordersSize(styles) {
    for (var _len = arguments.length, positions = Array(_len > 1 ? _len - 1 : 0), _key = 1; _key < _len; _key++) {
        positions[_key - 1] = arguments[_key];
    }

    return positions.reduce(function (size, pos) {
        var value = styles['border-' + pos + '-width'];

        return size + pixelsToNumber(value);
    }, 0);
}

/**
 * Extracts paddings sizes from provided styles.
 *
 * @param {CSSStyleDeclaration} styles
 * @returns {Object} Paddings box.
 */
function getPaddings(styles) {
    var boxKeys = ['top', 'right', 'bottom', 'left'];
    var paddings = {};

    for (var _iterator = boxKeys, _isArray = Array.isArray(_iterator), _i = 0, _iterator = _isArray ? _iterator : _iterator[Symbol.iterator]();;) {
        var _ref;

        if (_isArray) {
            if (_i >= _iterator.length) break;
            _ref = _iterator[_i++];
        } else {
            _i = _iterator.next();
            if (_i.done) break;
            _ref = _i.value;
        }

        var key = _ref;

        var value = styles['padding-' + key];

        paddings[key] = pixelsToNumber(value);
    }

    return paddings;
}

/**
 * Creates content rectangle based on the provided dimensions
 * and the top/left positions.
 *
 * @param {Number} width - Width of rectangle.
 * @param {Number} height - Height of rectangle.
 * @param {Number} top - Top position.
 * @param {Number} left - Left position.
 * @returns {ClientRect}
 */
function createContentRect(width, height, top, left) {
    return {
        width: width, height: height, top: top,
        right: width + left,
        bottom: height + top,
        left: left
    };
}

/**
 * Calculates content rectangle of provided SVG element.
 *
 * @param {SVGElement} target - Element whose content
 *      rectangle needs to be calculated.
 * @returns {ClientRect}
 */
function getSVGContentRect(target) {
    var bbox = target.getBBox();

    return createContentRect(bbox.width, bbox.height, 0, 0);
}

/**
 * Calculates content rectangle of a root element.
 *
 * @returns {ClientRect}
 */
function getDocElementRect() {
    // Neither scroll[Width/Height] nor offset[Width/Height] can be used to
    // define content dimensions as they give inconsistent results across
    // different browsers. E.g. in the Internet Explorer 10 and lower value of
    // these properties can't be less than the client dimensions (same thing
    // with the "getBoundingClientRect" method). And Firefox has the same
    // behavior with its "scroll" properties.
    var styles = getStyles(document.documentElement);

    var width = pixelsToNumber(styles.width);
    var height = pixelsToNumber(styles.height);

    return createContentRect(width, height, 0, 0);
}

/**
 * Calculates content rectangle of provided HTMLElement.
 *
 * @param {HTMLElement} target - Element whose content
 *      rectangle needs to be calculated.
 * @returns {ClientRect}
 */
function getHTMLElementContentRect(target) {
    // Client width & height properties can't be
    // used exclusively as they provide rounded values.
    var clientWidth = target.clientWidth,
        clientHeight = target.clientHeight;

    // By this condition we can catch all non-replaced inline, hidden and detached
    // elements. Though elements with width & height properties less than 0.5 will
    // be discarded as well.
    //
    // Without it we would need to implement separate methods for each of
    // those cases and it's not possible to perform a precise and performance
    // effective test for hidden elements. E.g. even jQuerys' ':visible' filter
    // gives wrong results for elements with width & height less than 0.5.

    if (!clientWidth && !clientHeight) {
        return emptyRect;
    }

    var styles = getStyles(target);
    var paddings = getPaddings(styles);
    var horizPad = paddings.left + paddings.right;
    var vertPad = paddings.top + paddings.bottom;

    // Computed styles of width & height are being used because they are the
    // only dimensions available to JS that contain non-rounded values. It could
    // be possible to utilize getBoundingClientRect if only its' data wasn't
    // affected by CSS transformations let alone paddings, borders and scroll bars.
    var width = pixelsToNumber(styles.width),
        height = pixelsToNumber(styles.height);

    // Width & height include paddings and borders
    // when 'border-box' box model is applied (except for IE).
    if (styles.boxSizing === 'border-box') {
        // Following conditions are required to handle Internet Explorer which
        // doesn't include paddings and borders to computed CSS dimensions.
        //
        // We can say that if CSS dimensions + paddings are equal to the "client"
        // properties then it's either IE, and thus we don't need to subtract
        // anything, or an element merely doesn't have paddings/borders styles.
        if (Math.round(width + horizPad) !== clientWidth) {
            width -= getBordersSize(styles, 'left', 'right') + horizPad;
        }

        if (Math.round(height + vertPad) !== clientHeight) {
            height -= getBordersSize(styles, 'top', 'bottom') + vertPad;
        }
    }

    // In some browsers (only in Firefox, actually) CSS width & height
    // include scroll bars size which can be removed at this step as scroll bars
    // are the only difference between rounded dimensions + paddings and "client"
    // properties, though that is not always true in Chrome.
    var vertScrollbar = Math.round(width + horizPad) - clientWidth;
    var horizScrollbar = Math.round(height + vertPad) - clientHeight;

    // Chrome has a rather weird rounding of "client" properties.
    // E.g. for an element with content width of 314.2px it sometimes gives the
    // client width of 315px and for the width of 314.7px it may give 314px.
    // And it doesn't happen all the time. Such difference needs to be ignored.
    if (Math.abs(vertScrollbar) !== 1) {
        width -= vertScrollbar;
    }

    if (Math.abs(horizScrollbar) !== 1) {
        height -= horizScrollbar;
    }

    return createContentRect(width, height, paddings.top, paddings.left);
}

/**
 * Checks whether provided element is an instance of SVGElement.
 *
 * @param {Element} target - Element to be checked.
 * @returns {Boolean}
 */
function isSVGElement(target) {
    return target instanceof SVGElement;
}

/**
 * Checks whether provided element is a document element (root element of a document).
 *
 * @param {Element} target - Element to be checked.
 * @returns {Boolean}
 */
function isDocumentElement(target) {
    return target === document.documentElement;
}

/**
 * Calculates an appropriate content rectangle for provided html or svg element.
 *
 * @param {Element} target - Element whose content rectangle
 *      needs to be calculated.
 * @returns {ClientRect}
 */
function getContentRect(target) {
    // Return empty rectangle if running in a non-browser environment.
    if (!isBrowser) {
        return emptyRect;
    }

    if (isSVGElement(target)) {
        return getSVGContentRect(target);
    }

    if (isDocumentElement(target)) {
        return getDocElementRect();
    }

    return getHTMLElementContentRect(target);
}

/**
 * Class that is responsible for computations of the content rectangle of
 * provided DOM element and for keeping track of its' changes.
 */
var ResizeObservation = function () {
    /**
     * Creates an instance of ResizeObservation.
     *
     * @param {Element} target - Element to be observed.
     */
    function ResizeObservation(target) {
        classCallCheck(this, ResizeObservation);

        this.target = target;

        // Keeps reference to the last observed content rectangle.
        this._contentRect = emptyRect;

        // Broadcasted width of content rectangle.
        this.broadcastWidth = 0;

        // Broadcasted height of content rectangle.
        this.broadcastHeight = 0;
    }

    /**
     * Updates 'broadcastWidth' and 'broadcastHeight' properties with a data
     * from the corresponding properties of the last observed content rectangle.
     *
     * @returns {ClientRect} Last observed content rectangle.
     */
    ResizeObservation.prototype.broadcastRect = function broadcastRect() {
        var rect = this._contentRect;

        this.broadcastWidth = rect.width;
        this.broadcastHeight = rect.height;

        return rect;
    };

    /**
     * Updates content rectangle and tells whether its' width or height properties
     * have changed since the last broadcast.
     *
     * @returns {Boolean}
     */
    ResizeObservation.prototype.isActive = function isActive() {
        var rect = getContentRect(this.target);

        this._contentRect = rect;

        return rect.width !== this.broadcastWidth || rect.height !== this.broadcastHeight;
    };

    return ResizeObservation;
}();

/**
 * Defines properties of the provided target object.
 *
 * @param {Object} target - Object for which to define properties.
 * @param {Object} props - Properties to be defined.
 * @param {Object} [descr = {}] - Properties descriptor.
 * @returns {Object} Target object.
 */
function defineProperties(target, props) {
    var descr = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : {};

    var descriptor = {
        configurable: descr.configurable || false,
        writable: descr.writable || false,
        enumerable: descr.enumerable || false
    };

    for (var _iterator = Object.keys(props), _isArray = Array.isArray(_iterator), _i = 0, _iterator = _isArray ? _iterator : _iterator[Symbol.iterator]();;) {
        var _ref;

        if (_isArray) {
            if (_i >= _iterator.length) break;
            _ref = _iterator[_i++];
        } else {
            _i = _iterator.next();
            if (_i.done) break;
            _ref = _i.value;
        }

        var key = _ref;

        descriptor.value = props[key];

        Object.defineProperty(target, key, descriptor);
    }

    return target;
}

var ResizeObserverEntry =
/**
 * Creates an instance of ResizeObserverEntry.
 *
 * @param {Element} target - Element that is being observed.
 * @param {ClientRect} rectData - Data of the elements' content rectangle.
 */
function ResizeObserverEntry(target, rectData) {
    classCallCheck(this, ResizeObserverEntry);

    // Content rectangle needs to be an instance of ClientRect if it's
    // available.
    var rectInterface = global$1.ClientRect || Object;
    var contentRect = Object.create(rectInterface.prototype);

    // According to the specification following properties are not writable
    // and are also not enumerable in the native implementation.
    //
    // Property accessors are not being used as they'd require to define a
    // private WeakMap storage which may cause memory leaks in browsers that
    // don't support this type of collections.
    defineProperties(contentRect, rectData, { configurable: true });

    defineProperties(this, {
        target: target, contentRect: contentRect
    }, { configurable: true });
};

var ResizeObserver$2 = function () {
    /**
     * Creates a new instance of ResizeObserver.
     *
     * @param {Function} callback - Callback function that is invoked when one
     *      of the observed elements changes it's content rectangle.
     * @param {ResizeObsreverController} controller - Controller instance which
     *      is responsible for the updates of observer.
     * @param {ResizeObserver} publicObserver - Reference to the public
     *      ResizeObserver instance which will be passed to callback function.
     */
    function ResizeObserver(callback, controller, publicObserver) {
        classCallCheck(this, ResizeObserver);

        if (typeof callback !== 'function') {
            throw new TypeError('The callback provided as parameter 1 is not a function.');
        }

        // Reference to the callback function.
        this._callback = callback;

        // Registry of ResizeObservation instances.
        this._targets = new Map();

        // Collection of resize observations that have detected changes in
        // dimensions of elements.
        this._activeTargets = [];

        // Reference to the associated ResizeObserverController.
        this._controller = controller;

        // Public ResizeObserver instance which will be passed to callback function.
        this._publicObserver = publicObserver;
    }

    /**
     * Starts observing provided element.
     *
     * @param {Element} target - Element to be observed.
     */
    ResizeObserver.prototype.observe = function observe(target) {
        if (!arguments.length) {
            throw new TypeError('1 argument required, but only 0 present.');
        }

        // Do nothing if current environment doesn't have the Element interface.
        if (!('Element' in global$1) || !(Element instanceof Object)) {
            return;
        }

        if (!(target instanceof Element)) {
            throw new TypeError('parameter 1 is not of type "Element".');
        }

        var targets = this._targets;

        // Do nothing if element is already being observed.
        if (targets.has(target)) {
            return;
        }

        // Register new ResizeObservation instance.
        targets.set(target, new ResizeObservation(target));

        // Add observer to controller if it hasn't been connected yet.
        if (!this._controller.isConnected(this)) {
            this._controller.connect(this);
        }

        // Update observations.
        this._controller.refresh();
    };

    /**
     * Stops observing provided element.
     *
     * @param {Element} target - Element to stop observing.
     */
    ResizeObserver.prototype.unobserve = function unobserve(target) {
        if (!arguments.length) {
            throw new TypeError('1 argument required, but only 0 present.');
        }

        // Do nothing if current environment doesn't have the Element interface.
        if (!('Element' in global$1) || !(Element instanceof Object)) {
            return;
        }

        if (!(target instanceof Element)) {
            throw new TypeError('parameter 1 is not of type "Element".');
        }

        var targets = this._targets;

        // Do nothing if element is not being observed.
        if (!targets.has(target)) {
            return;
        }

        // Remove element and associated with it ResizeObsrvation instance from
        // registry.
        targets.delete(target);

        // Set back the initial state if there is nothing to observe.
        if (!targets.size) {
            this.disconnect();
        }
    };

    /**
     * Stops observing all elements and clears the observations list.
     */
    ResizeObserver.prototype.disconnect = function disconnect() {
        this.clearActive();
        this._targets.clear();
        this._controller.disconnect(this);
    };

    /**
     * Clears an array of previously collected active observations and collects
     * observation instances which associated element has changed its' content
     * rectangle.
     */
    ResizeObserver.prototype.gatherActive = function gatherActive() {
        this.clearActive();

        var activeTargets = this._activeTargets;

        this._targets.forEach(function (observation) {
            if (observation.isActive()) {
                activeTargets.push(observation);
            }
        });
    };

    /**
     * Invokes initial callback function with a list of ResizeObserverEntry
     * instances collected from active resize observations.
     */
    ResizeObserver.prototype.broadcastActive = function broadcastActive() {
        // Do nothing if observer doesn't have active observations.
        if (!this.hasActive()) {
            return;
        }

        var observer = this._publicObserver;

        // Create ResizeObserverEntry instance for every active observation.
        var entries = this._activeTargets.map(function (observation) {
            return new ResizeObserverEntry(observation.target, observation.broadcastRect());
        });

        this.clearActive();
        this._callback.call(observer, entries, observer);
    };

    /**
     * Clears the collection of pending/active observations.
     */
    ResizeObserver.prototype.clearActive = function clearActive() {
        this._activeTargets.splice(0);
    };

    /**
     * Tells whether observer has pending observations.
     *
     * @returns {Boolean}
     */
    ResizeObserver.prototype.hasActive = function hasActive() {
        return !!this._activeTargets.length;
    };

    return ResizeObserver;
}();

/**
 * A shim for requestAnimationFrame which falls back
 * to setTimeout if the first one is not supported.
 *
 * @returns {Number} Requests' identifier.
 */
var requestAnimFrame = (function () {
    if (typeof requestAnimationFrame === 'function') {
        return requestAnimationFrame;
    }

    return function (callback) {
        return setTimeout(function () {
            return callback(Date.now());
        }, 1000 / 60);
    };
})();

/**
 * Creates a wrapper function that ensures that provided callback will
 * be invoked only once during the specified delay period. It caches the last
 * call and re-invokes it after pending activation is resolved.
 *
 * @param {Function} callback - Function to be invoked after the delay period.
 * @param {Number} [delay = 0] - Delay after which to invoke callback.
 * @param {Boolean} [afterRAF = false] - Whether function needs to be invoked as
 *      a requestAnimationFrame callback.
 * @returns {Function}
 */
var throttle = function (callback) {
    var delay = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 0;
    var afterRAF = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : false;

    var leadCall = null,
        edgeCall = null;

    /**
     * Invokes the original callback function and schedules a new invocation if
     * the wrapper was called during current request.
     */
    function invokeCallback() {
        // Invoke original function.
        callback.apply.apply(callback, leadCall);

        leadCall = null;

        // Schedule new invocation if there was a call during delay period.
        if (edgeCall) {
            proxy.apply.apply(proxy, edgeCall);

            edgeCall = null;
        }
    }

    /**
     * Callback that will be invoked after the specified delay period. It will
     * delegate invocation of the original function to the requestAnimationFrame
     * if "afterRAF" parameter is set to "true".
     */
    function timeoutCallback() {
        afterRAF ? requestAnimFrame(invokeCallback) : invokeCallback();
    }

    /**
     * Schedules invocation of the initial function.
     */
    function proxy() {
        for (var _len = arguments.length, args = Array(_len), _key = 0; _key < _len; _key++) {
            args[_key] = arguments[_key];
        }

        // eslint-disable-next-line no-invalid-this
        var callData = [this, args];

        // Cache current call to be re-invoked later if there is already a
        // pending call.
        if (leadCall) {
            edgeCall = callData;
        } else {
            leadCall = callData;

            // Schedule new invocation.
            setTimeout(timeoutCallback, delay);
        }
    }

    return proxy;
};

// Define whether the MutationObserver is supported.
var mutationsSupported = typeof MutationObserver === 'function';

/**
 * Controller class which handles updates of ResizeObserver instances.
 * It decides when and for how long it's necessary to run updates by listening
 * to the windows "resize" event along with a tracking of DOM mutations
 * (nodes removal, changes of attributes, etc.).
 *
 * Transitions and animations are handled by running a repeatable update cycle
 * until the dimensions of observed elements are changing.
 *
 * Continuous update cycle will be used automatically in case MutationObserver
 * is not supported.
 */
var ResizeObserverController = function () {
    /**
     * Creates a new instance of ResizeObserverController.
     *
     * @param {Boolean} [continuousUpdates = false] - Whether to use a continuous
     *      update cycle.
     */
    function ResizeObserverController() {
        var continuousUpdates = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : false;
        classCallCheck(this, ResizeObserverController);

        // Continuous updates must be enabled if MutationObserver is not supported.
        this._isCycleContinuous = !mutationsSupported || continuousUpdates;

        // Indicates whether DOM listeners have been added.
        this._listenersEnabled = false;

        // Keeps reference to the instance of MutationObserver.
        this._mutationsObserver = null;

        // A list of connected observers.
        this._observers = [];

        // Make sure that the "refresh" method is invoked as a RAF callback and
        // that it happens only once during the period of 30 milliseconds.
        this.refresh = throttle(this.refresh.bind(this), 30, true);

        // Additionally postpone invocation of the continuous updates.
        this._continuousUpdateHandler = throttle(this.refresh, 70);
    }

    /**
     * Adds observer to observers list.
     *
     * @param {ResizeObserver} observer - Observer to be added.
     */
    ResizeObserverController.prototype.connect = function connect(observer) {
        if (!this.isConnected(observer)) {
            this._observers.push(observer);
        }

        // Add listeners if they haven't been added yet.
        if (!this._listenersEnabled) {
            this._addListeners();
        }
    };

    /**
     * Removes observer from observers list.
     *
     * @param {ResizeObserver} observer - Observer to be removed.
     */
    ResizeObserverController.prototype.disconnect = function disconnect(observer) {
        var observers = this._observers;
        var index = observers.indexOf(observer);

        // Remove observer if it's present in registry.
        if (~index) {
            observers.splice(index, 1);
        }

        // Remove listeners if controller has no connected observers.
        if (!observers.length && this._listenersEnabled) {
            this._removeListeners();
        }
    };

    /**
     * Tells whether the provided observer is connected to controller.
     *
     * @param {ResizeObserver} observer - Observer to be checked.
     * @returns {Boolean}
     */
    ResizeObserverController.prototype.isConnected = function isConnected(observer) {
        return !!~this._observers.indexOf(observer);
    };

    /**
     * Invokes the update of observers. It will continue running updates insofar
     * it detects changes or if continuous updates are enabled.
     */
    ResizeObserverController.prototype.refresh = function refresh() {
        var hasChanges = this._updateObservers();

        // Continue running updates if changes have been detected as there might
        // be future ones caused by CSS transitions.
        if (hasChanges) {
            this.refresh();
        } else if (this._isCycleContinuous && this._listenersEnabled) {
            // Automatically repeat cycle if it's necessary.
            this._continuousUpdateHandler();
        }
    };

    /**
     * Updates every observer from observers list and notifies them of queued
     * entries.
     *
     * @private
     * @returns {Boolean} Returns "true" if any observer has detected changes in
     *      dimensions of its' elements.
     */
    ResizeObserverController.prototype._updateObservers = function _updateObservers() {
        var hasChanges = false;

        for (var _iterator = this._observers, _isArray = Array.isArray(_iterator), _i = 0, _iterator = _isArray ? _iterator : _iterator[Symbol.iterator]();;) {
            var _ref;

            if (_isArray) {
                if (_i >= _iterator.length) break;
                _ref = _iterator[_i++];
            } else {
                _i = _iterator.next();
                if (_i.done) break;
                _ref = _i.value;
            }

            var observer = _ref;

            // Collect active observations.
            observer.gatherActive();

            // Broadcast active observations and set the flag that changes have
            // been detected.
            if (observer.hasActive()) {
                hasChanges = true;

                observer.broadcastActive();
            }
        }

        return hasChanges;
    };

    /**
     * Initializes DOM listeners.
     *
     * @private
     */
    ResizeObserverController.prototype._addListeners = function _addListeners() {
        // Do nothing if running in a non-browser environment or if listeners
        // have been already added.
        if (!isBrowser || this._listenersEnabled) {
            return;
        }

        window.addEventListener('resize', this.refresh);

        // Subscribe to DOM mutations if it's possible as they may lead to
        // changes in the dimensions of elements.
        if (mutationsSupported) {
            this._mutationsObserver = new MutationObserver(this.refresh);

            this._mutationsObserver.observe(document, {
                attributes: true,
                childList: true,
                characterData: true,
                subtree: true
            });
        }

        this._listenersEnabled = true;

        // Don't wait for a possible event that might trigger the update of
        // observers and manually initiate the update process.
        if (this._isCycleContinuous) {
            this.refresh();
        }
    };

    /**
     * Removes DOM listeners.
     *
     * @private
     */
    ResizeObserverController.prototype._removeListeners = function _removeListeners() {
        // Do nothing if running in a non-browser environment or if listeners
        // have been already removed.
        if (!isBrowser || !this._listenersEnabled) {
            return;
        }

        window.removeEventListener('resize', this.refresh);

        if (this._mutationsObserver) {
            this._mutationsObserver.disconnect();
        }

        this._mutationsObserver = null;
        this._listenersEnabled = false;
    };

    createClass(ResizeObserverController, [{
        key: 'continuousUpdates',

        /**
         * Tells whether continuous updates are enabled.
         *
         * @returns {Boolean}
         */
        get: function get() {
            return this._isCycleContinuous;
        },

        /**
         * Enables or disables continuous updates.
         *
         * @param {Boolean} useContinuous - Whether to enable or disable continuous
         *      updates. Note that the value won't be applied if MutationObserver is
         *      not supported.
         */
        set: function set(useContinuous) {
            // The state of continuous updates should not be modified if
            // MutationObserver is not supported.
            if (!mutationsSupported) {
                return;
            }

            this._isCycleContinuous = useContinuous;

            // Immediately start the update cycle in order not to wait for a possible
            // event that might initiate it.
            if (this._listenersEnabled && useContinuous) {
                this.refresh();
            }
        }
    }]);
    return ResizeObserverController;
}();

// Controller that will be assigned to all instances of ResizeObserver.
var controller = new ResizeObserverController();

// Registry of the internal observers.
var observers = new WeakMap();

/**
 * ResizeObservers' "Proxy" class which is meant to hide private properties and
 * methods from public instances.
 *
 * Additionally implements the "continuousUpdates" static property accessor to
 * give control over the behavior of the ResizeObserverController instance.
 * Changes made to this property affect all future and existing observers.
 */
var ResizeObserver = function () {
    /**
     * Creates a new instance of ResizeObserver.
     *
     * @param {Function} callback - Callback that is invoked when dimensions of
     *      one of the observed elements change.
     */
    function ResizeObserver(callback) {
        classCallCheck(this, ResizeObserver);

        if (!arguments.length) {
            throw new TypeError('1 argument required, but only 0 present.');
        }

        // Create a new instance of the internal ResizeObserver.
        var observer = new ResizeObserver$2(callback, controller, this);

        // Register internal observer.
        observers.set(this, observer);
    }

    createClass(ResizeObserver, null, [{
        key: 'continuousUpdates',

        /**
         * Tells whether continuous updates are enabled.
         *
         * @returns {Boolean}
         */
        get: function get() {
            return controller.continuousUpdates;
        },

        /**
         * Enables or disables continuous updates.
         *
         * @param {Boolean} value - Whether to enable or disable continuous updates.
         */
        set: function set(value) {
            if (typeof value !== 'boolean') {
                throw new TypeError('type of "continuousUpdates" value must be boolean.');
            }

            controller.continuousUpdates = value;
        }
    }]);
    return ResizeObserver;
}();

// Expose public methods of ResizeObserver.
['observe', 'unobserve', 'disconnect'].forEach(function (method) {
    ResizeObserver.prototype[method] = function () {
        var _observers$get;

        return (_observers$get = observers.get(this))[method].apply(_observers$get, arguments);
    };
});

var index = (function () {
    // Export existing implementation if it's available.
    if (typeof global$1.ResizeObserver === 'function') {
        return global$1.ResizeObserver;
    }

    return ResizeObserver;
})();

return index;
})));

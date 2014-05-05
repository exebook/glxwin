var glxwin = require('./build/Release/glxwin.node');
TGLXWin = kindof();

/**
 *
 * @param parent
 * @param fontName
 * @param fontSize
 * @param extraX
 * @param tuneY
 */
TGLXWin.can.init = function(parent, fontName, fontSize, extraX, tuneY) {
    this.handle = glxwin.create_win(undefined, fontName, fontSize, extraX, tuneY);
    if (glxwin.mainWindow == undefined) {
        glxwin.mainWindow = this;
    }
    if (glxwin.all == undefined) {
        glxwin.all = [this];
    } else {
        glxwin.all.push(this);
    }
};

/**
 * Resize or move current window
 * @param {number} x
 * @param {number} y
 * @param {number} w
 * @param {number} h
 */
TGLXWin.can.setXYWH = function(x, y, w, h) {
    glxwin.set_xywh(this.handle, x, y, w, h);
};

/**
 * Get current window position
 * @return {*}
 */
TGLXWin.can.getXYWH = function() {
    return glxwin.get_xywh(this.handle);
};

/**
 * Show current window
 */
TGLXWin.can.show = function() {
    glxwin.show(this.handle);
};

/**
 * Hide current window
 */
TGLXWin.can.hide = function() {
    glxwin.hide(this.handle);
};

/**
 * Check new events in windows. If have new events which needs to be calculated it will return true
 * @return {*}
 */
TGLXWin.can.step = function() {
    return glxwin.step(this.handle);
};

/**
 * Draw a rectangle
 * @param {number} x1 Coordinate of rectangle
 * @param {number} y1 Coordinate of rectangle
 * @param {number} x2 Coordinate of rectangle
 * @param {number} y2 Coordinate of rectangle
 * @param {number} color
 */
TGLXWin.can.crect = function(x1, y1, x2, y2, color) {
    glxwin.crect(this.handle, x1, y1, x2, y2, color);
};

/**
 * Increase draw counter for current window
 */
TGLXWin.can.repaint = function() {
    glxwin.repaint(this.handle);
};

/**
 * Forced redrawing current window ignoring draw counter
 */
TGLXWin.can.forceRepaint = function() {
    glxwin.force_repaint(this.handle);
};



/**
 *
 * @param {number} x Coordinate X for drawing text sets in Pixels
 * @param {number} y Coordinate Y for drawing text sets in Pixels
 * @param {number} w Size in characters two-dimensional array of letters
 * @param {number} h Size in characters two-dimensional array of letters
 * @param {Array} TXT Array of texts
 * @param {Array} CLR Array of colors
 */
TGLXWin.can.colorText = function(x, y, w, h, TXT, CLR) {
    glxwin.color_text(this.handle, x, y, w, h, TXT, CLR);
};

/**
 * For begin code printing.(Sometimes it need depending on platform)
 */
TGLXWin.can.paintBegin = function() {
    glxwin.paintBegin(this.handle);
};

/**
 * For end code printing.(Sometimes it need depending on platform)
 */
TGLXWin.can.paintEnd = function() {
    glxwin.paintEnd(this.handle);
};

/**
 * Set cursor to current windows.
 * @param {number} cursor   0 - pointer
 *                          1 - text
 *                          2 - up-to-down
 *                          3 - move
 *                          4 - hand1
 *                          5 - hand2
 *                          6 - grab
 */
TGLXWin.can.setCursor = function(cursor) {
    glxwin.setCursor(this.handle, cursor);
};

/**
 * Set step for resize window.
 * @param {number} w
 * @param {number} h
 */
TGLXWin.can.setSizeSteps = function(w, h) {
    glxwin.setSizeSteps(this.handle, w, h);
};

/**
 *
 * @param hand
 */
glxwin.dispatch = function(hand) {
    var O = this.findObject(hand.handle);
    if (hand.call == 'onPaint') {
        if (O.onPaint != undefined) {
            O.paintBegin();
            O.onPaint();
            O.paintEnd();
        }
    } else if (hand.call == 'onKey') {
        if (O.onKey != undefined) {
            O.onKey(hand.down, hand.char, hand.key, hand.physical);
        }
    } else {
        log('dispatch error');
    }
};

/**
 *
 * @param handle
 * @return {*}
 */
glxwin.findObject = function(handle) {
    for (var i = 0; i < this.all.length; i++) {
        if (this.all[i].handle == handle) {
            return this.all[i];
        }
    }
};

/**
 *
 * @param handle
 * @param x
 * @param y
 */
glxwin.onCursor = function(handle, x, y) {
    var O = this.findObject(handle);
    if (O.onCursor != undefined) {
        O.onCursor(x, y);
    }
};

/**
 *
 * @param handle
 * @param w
 * @param h
 */
glxwin.onSize = function(handle, w, h) {
    var O = this.findObject(handle);
    if (O.onSize != undefined) {
        O.onSize(w, h);
    }
};

/**
 *
 * @param handle
 * @param button
 * @param down
 * @param x
 * @param y
 */
glxwin.onMouse = function(handle, button, down, x, y) {
    var O = this.findObject(handle);
    if (O.onMouse != undefined) {
        O.onMouse(button, down, x, y);
    }
};

/**
 *
 * @param handle
 * @param on
 */
glxwin.onFocus = function(handle, on) {
    var O = this.findObject(handle);
    if (O.onFocus != undefined) {
        if (O.onFocus(on)) O.repaint();
    }
};

/**
 *
 */
glxwin.onPipe = function() {
    var O = this.findObject(handle);
    if (O.onPipe != undefined) {
        O.onPipe();
    }
};

glxwin.register_callbacks(glxwin);

/**
 *
 */
glxwin.mainLoop = function() {
    var speed = 100, timer;

    /**
     *
     */
    function slowDown() {
        speed = 100;
        timer = undefined;
    }

    /**
     *
     */
    function go() {
        if (glxwin.x11quit()) {
            process.exit();
            return;
        }

        var event = glxwin.step();
        if (event) {
            speed = 1;
            if (timer) {
                clearTimeout(timer);
            }
            timer = setTimeout(slowDown, 1000);
            var A = glxwin['c++callbacks'];

            while (A.length > 0) {
                glxwin.dispatch(A.shift());
            }
        }
        setTimeout(function() {
            setImmediate(go);
        }, speed);
    }

    var AR = glxwin['c++callbacks'];

    /**
     *
     */
    function rend() {
        if (glxwin.x11quit()) {
            return;
        }
        glxwin.step_renders();
        while (AR.length > 0) {
            glxwin.dispatch(AR.shift());
        }
        setTimeout(rend, 20);
    }

    setImmediate(go);
    setImmediate(rend);
};

module.exports = glxwin;

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
TGLXWin.can.init = function (parent, fontName, fontSize, extraX, tuneY) {
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
 *
 * @param x
 * @param y
 * @param w
 * @param h
 */
TGLXWin.can.setXYWH = function (x, y, w, h) {
    glxwin.set_xywh(this.handle, x, y, w, h);
};

/**
 *
 * @returns {*}
 */
TGLXWin.can.getXYWH = function () {
    return glxwin.get_xywh(this.handle);
};

/**
 *
 */
TGLXWin.can.show = function () {
    glxwin.show(this.handle);
};

/**
 *
 * @returns {*}
 */
TGLXWin.can.step = function () {
    return glxwin.step(this.handle);
};

/**
 *
 * @param x
 * @param y
 * @param w
 * @param h
 * @param color
 */
TGLXWin.can.crect = function (x, y, w, h, color) {
    glxwin.crect(this.handle, x, y, w, h, color);
};

/**
 *
 * @param text
 * @param x
 * @param y
 */
TGLXWin.can.print = function (text, x, y) {
    glxwin.print(this.handle, text, x, y);
};

/**
 *
 * @param color
 */
TGLXWin.can.fontColor = function (color) {
    glxwin.font_color(this.handle, color);
};

/**
 *
 * @param name
 * @param size
 * @param color
 * @param bgcolor
 * @returns {*}
 */
TGLXWin.can.applyFont = function (name, size, color, bgcolor) {
    return glxwin.apply_font(this.handle, name, size, color, bgcolor);
};

/**
 *
 * @param txt
 * @returns {*}
 */
TGLXWin.can.textExtent = function (txt) {
    return glxwin.text_extent(this.handle, txt);
};

/**
 *
 */
TGLXWin.can.repaint = function () {
    glxwin.repaint(this.handle);
};

/**
 *
 */
TGLXWin.can.forceRepaint = function () {
    glxwin.force_repaint(this.handle);
};

/**
 *
 */
TGLXWin.can.hide = function () {
    glxwin.hide(this.handle);
};

/**
 *
 * @param x
 * @param y
 * @param w
 * @param h
 * @param TXT
 * @param CLR
 */
TGLXWin.can.colorText = function (x, y, w, h, TXT, CLR) {
    glxwin.color_text(this.handle, x, y, w, h, TXT, CLR);
};

/**
 *
 * @returns {*}
 * not tested yet
 */
TGLXWin.can.getXWindowHandle = function () {
    return glxwin.get_xwindow_handle(this.handle);
};

/**
 *
 */
TGLXWin.can.paintBegin = function () {
    glxwin.paintBegin(this.handle);
};

/**
 *
 */
TGLXWin.can.paintEnd = function () {
    glxwin.paintEnd(this.handle);
};

/**
 *
 * @param cursor
 */
TGLXWin.can.setCursor = function (cursor) {
    glxwin.setCursor(this.handle, cursor);
};

/**
 *
 * @param w
 * @param h
 */
TGLXWin.can.setSizeSteps = function (w, h) {
    glxwin.setSizeSteps(this.handle, w, h)
};

/**
 *
 * @param hand
 */
glxwin.dispatch = function (hand) {
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
 * @returns {*}
 */
glxwin.findObject = function (handle) {
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
glxwin.onCursor = function (handle, x, y) {
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
glxwin.onSize = function (handle, w, h) {
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
glxwin.onMouse = function (handle, button, down, x, y) {
    var O = this.findObject(handle);
    if (O.onMouse != undefined) {
        O.onMouse(button, down, x, y);
    }
}

/*
glxwin.onKey = function (handle, down, char, key, physical) {
	var O  = this.findObject(handle)
	if (O.onKey != undefined) O.onKey(down, char, key, physical)
};
*/

/**
 *
 * @param handle
 * @param on
 */
glxwin.onFocus = function (handle, on) {
    var O = this.findObject(handle);
    if (O.onFocus != undefined) {
        if (O.onFocus(on)) O.repaint();
    }
};

/*
glxwin.onPaint = function (handle) {
	var O  = this.findObject(handle)
	if (O.onPaint != undefined) O.onPaint()
};
*/

/**
 *
 */
glxwin.onPipe = function () {
    var O = this.findObject(handle);
    if (O.onPipe != undefined) {
        O.onPipe();
    }
};

glxwin.register_callbacks(glxwin);

/**
 *
 */
glxwin.mainLoop = function () {
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
                glxwin.dispatch(A.shift())
            }
        }
        setTimeout(function () {
            setImmediate(go);
        }, speed)
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

/*
 // MINIMAL PROGRAM
 glxwin = require('./glxwin/glxwin.js')

 var w = TGLXWin.create()
 w.setXYWH(0, 0, 100, 100)
 w.onKey = function(K) {
 console.log(K)
 }
 w.onPaint = function() {
 w.crect(0, 0, 1000, 1000, 0xff000000)
 }
 w.show()
 glxwin.mainLoop()
 */
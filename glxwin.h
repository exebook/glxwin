/*
    This class is supposed to provide a simple interface to a window
    It should isolate all the mess of Xlib, GLX, XIM, FreeType, OpenGL
    it must provide:
+	1) window creation/destruction
+	2) show/hide
+	3) mouse cursor/button callbacks
+	4) raw keyboard and char input callbacks
+	5) timers set, clear, callback
+	6) set size, pos, and onsize callback
		6.1) - get size/pos?
+	7) simple graphix output color crect()
+	8) unicode text output with print(), calc size of string
+	9) select font and color for text
	10) create bitmap, draw it
	11) place one window into another during create() or with insert()
    consider:
    onhover, onenter/onleave, onminimize, onfocus/onunfocus, onclose, onmove
*/

						#include <X11/X.h>
						#include <X11/Xlib.h>
						#include <GL/glx.h>
						#include <GL/gl.h>
							#include <ft2build.h>
							#include <freetype/freetype.h>
							#include FT_FREETYPE_H
						#define UNICODE
						#include "libosnova.cpp"
//						#include "nova.h"
						#include "utf.cpp"
						#include "yaglfont.h"
   int glAttrib[] = {
      GLX_DRAWABLE_TYPE , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE   , GLX_RGBA_BIT,
      GLX_RED_SIZE      , 1,
      GLX_GREEN_SIZE    , 1,
      GLX_BLUE_SIZE     , 1,
      GLX_ALPHA_SIZE    , 8,
      GLX_DEPTH_SIZE    , 24,
//      GLX_DOUBLEBUFFER  , True,
      None
   };

struct win;
#  define  on_paint    void render()
#  define  on_cursor   void cursor(int x, int y)
#  define  on_mouse    void mouse(bool down, int button, int x, int y)
#  define  on_created  void creating()
//#  define  on_char     void character(wchar_t ch)
#  define  on_key      void keyboard(bool down, int charcode, int key, bool physical)
#  define  on_size     void sizing(int w, int h)
#  define  on_timer    void timer(int id)
#  define  on_focus    void focus(bool on)

struct timer { win *w; int on, id, interval, next; };

struct mess {
	Display *d;
	int screen;
	Window root;
	GLXFBConfig *fbConfigs;
	XVisualInfo *visInfo;
	XIM im;
	XContext xcontext;
	arr <win*> all;
	list <timer> timers; int nearest_timer;
	list<yaglfont::Font> Fonts;

	mess():d(0), nearest_timer(0) { }
	~mess() {
		close();
		printf("shytdavn\n");
	}

	void init();
	bool run();
	void close();

	win* handle2win(Drawable handle) {
		win *wnd;
		XFindContext(d, handle, xcontext, (XPointer*)&wnd);
		return wnd;
	}

	void remove(win *w) {
		int i = all.find(w);
		if (i >= 0) all.del(i, 1);
	}

	void add_timer(win *w, int id, int interval) {
		del_timer(w, id);
		timers++;
		timers[-1].on = true;
		timers[-1].w = w;
		timers[-1].id = id;
		timers[-1].interval = interval;
		timers[-1].next = time1000() + interval;
		nearest_timer = 0;
	}

	void del_timer(win *w, int id) {
		each (i, timers) if (timers[i].on && timers[i].w == w && timers[i].id == id) {
			timers[i].on = false;
			break;
		}
		nearest_timer = 0;
	}

	void process_timers();
} MESS;

struct win {
//   Atom delMsg;
   GLXContext glx;
   GLXDrawable window;
   double x, y, w, h;
   XIC ic;
   str bla;
   int renders, curfont;

int ICON[16*16] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,
	1,2,1,2,1,2,2,1,1,2,1,1,1,1,2,1,
	1,2,2,1,2,1,2,1,1,2,2,2,2,2,2,1,
	1,2,3,3,3,2,2,1,1,2,2,2,2,2,2,1,
	1,2,1,2,1,2,2,1,1,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
}, ICONCOLOR[6] = {0, 0x44eeff, 0xff, 0xff0000, 0xffff00, 0xffff};

Pixmap load_pixmap() {
	int W = 16, H = 16;
	Pixmap pixmap = XCreatePixmap(MESS.d, window, W, H, 24);
	XGCValues gr_values;
	GC gr_context = XCreateGC(MESS.d, window, GCBackground, &gr_values);
	str PIX; PIX(W*H*4*4); PIX.fill(0);
	uint32_t *X = (uint32_t*) *PIX;
	XImage *ximage = XCreateImage(MESS.d, CopyFromParent, 24, XYPixmap, 0, (char*) X, W, H, 32, W);
	int i = 0;
	for (int y = 0; y < H; y++)
	for (int x = 0; x < W; x++)
		XPutPixel(ximage, x, y, ICONCOLOR[ICON[i++]]);
	XPutImage(MESS.d, pixmap, gr_context, ximage, 0, 0, 0, 0, W, H);
	return pixmap;
}

str DeodarTitle = w2utf(L"Деодар Коммандер");

void set_icon() {
	unsigned int icon_width, icon_height;
	char *window_name = "Deodar Commander";
	char *icon_name = "deodar";
	Pixmap icon_pixmap;
	XSizeHints size_hints;
	XWMHints wm_hints;
	XClassHint class_hints;
	XTextProperty windowName, iconName;
	icon_pixmap = load_pixmap();
	size_hints.flags = PPosition | PSize | PMinSize;
	size_hints.min_width = 300;
	size_hints.min_height = 200;
	XStringListToTextProperty(&window_name, 1, &windowName);
	XStringListToTextProperty(&icon_name, 1, &iconName);
	wm_hints.initial_state = NormalState;
	wm_hints.input = True;
	wm_hints.icon_pixmap = icon_pixmap;
	wm_hints.flags = StateHint | IconPixmapHint | InputHint;
	class_hints.res_name = "deodar";
	class_hints.res_class = "Deodar";
	XSetWMProperties(MESS.d, window, &windowName, &iconName, 0, 0, &size_hints, &wm_hints, &class_hints);
}

void create() {
	if (MESS.d == 0) {
		MESS.init();
	}
	renders = curfont = 0;
	w = 300;
	h = 200;
	XSetWindowAttributes windowAttr;
	windowAttr.colormap = XCreateColormap(MESS.d, MESS.root, MESS.visInfo->visual, AllocNone);
	window = XCreateWindow(MESS.d, MESS.root, 200,200, 300,200, 0, MESS.visInfo->depth, InputOutput, MESS.visInfo->visual, CWColormap, &windowAttr);
	glx = glXCreateContext(MESS.d, MESS.visInfo,  NULL, True);
   glXMakeCurrent(MESS.d, window, glx);
	ic = XCreateIC(MESS.im, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, window, NULL);
	if (ic == NULL) printf("Could not open IC\n");
	XSetICFocus(ic);
	XSelectInput(MESS.d, window, 0
		| StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask
		| ButtonReleaseMask |KeymapStateMask | PointerMotionMask | VisibilityChangeMask
		| KeymapStateMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask
	);
	set_icon();
	XSaveContext(MESS.d, window, MESS.xcontext, (XPointer)this);
	MESS.all << this;
	creating();
	//	XSetWMProtocols(d, W.win, &(delMsg), 1);
}

void show() {
   XMapWindow(MESS.d, window);
}

void hide() {
   XUnmapWindow(MESS.d, window);
}

void resetGL() {
   glXMakeCurrent(MESS.d, window, glx);
   glClearColor(1, 1, 1, 1.0);
	glViewport(0, 0, w, h);
	//	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDisable(GL_DITHER);
	//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
}

bool run() {
	return MESS.run();
}

void paint() {
	renders++;
}

void add_timer(int id, int T) {
	MESS.add_timer(this, id, T);
}

void del_timer(int id) {
	MESS.del_timer(this, id);
}

void size(int _w, int _h) {
	w = _w, h = _h;
	XResizeWindow(MESS.d, window, w, h);
}

void move(int x, int y) {
	XMoveWindow(MESS.d, window, x, y);
}

void print(wstr str, int x, int y) {
	glLoadIdentity();
	glTranslatef(x, y, 0);
	yaglfont::Font *f = & MESS.Fonts[curfont];
	glBindTexture( GL_TEXTURE_2D, f->T );
	draw_str(*f, str);
	glBindTexture( GL_TEXTURE_2D, -1 );
}

void print(char *s, int x, int y) {
	wstr w = utf2w(s);
	print(w, x, y);
}

int* text_extent(wstr s) {
	int *x = yaglfont::size_str(MESS.Fonts[curfont], s);
	return x;
}

void crect(int x, int y, int x1, int y1, int color) {
	glColor4ubv((GLubyte*)&color);
	glLoadIdentity();
	glBegin(GL_QUADS);
		glVertex2i(x, y);
		glVertex2i(x1, y);
		glVertex2i(x1, y1);
		glVertex2i(x, y1);
	glEnd();
}
void clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void flush() {
//	glXSwapBuffers(MESS.d, window);
//	glFinish();
	glFlush();
}

virtual on_paint  {}
virtual on_cursor {}
virtual on_mouse  {}
virtual on_created{}
//virtual on_char   {}
virtual on_key    {}
virtual on_size   {}
virtual on_timer  {}
virtual on_focus  {}
// translators:
void x_expose() {
	paint();
}
void x_destroy() {
	glXDestroyContext(MESS.d, glx);
	MESS.remove(this);
}
void x_resize(int _x, int _y, int _w, int _h) {
	w = _w, h = _h; x = _x, y = _y;
	resetGL();
	sizing(w, h);
}

void x_focus(bool on) {
	focus(on);
}

};




void mess::init() {
	d = XOpenDisplay(NULL);
	screen = XDefaultScreen(d);
	//   Atom delMsg = XInternAtom(d, "WM_DELETE_WINDOW", True);
   int num;
   fbConfigs = glXChooseFBConfig(d, screen, glAttrib, &num);
   if(fbConfigs == NULL) printf("Unable to find a frame buffer config!\n");
	root = XRootWindow(d, screen);
	visInfo = glXGetVisualFromFBConfig(d, fbConfigs[0]);
    XIMStyles *styles;
    XIMStyle xim_requested_style;
    im = XOpenIM(d, NULL, NULL, NULL);
    if (im == NULL) {
        fputs("Could not open input method\n", stdout);
    }
    char * failed_arg = XGetIMValues(im, XNQueryInputStyle, &styles, NULL);
    if (failed_arg != NULL) {
      fputs("XIM Can't get styles\n", stdout);
    }
    int i;
    for (i = 0; i < styles->count_styles; i++) {
//        printf("style %d\n", styles->supported_styles[i]);
    }
}

void mess::close() {
	if (d) {
		XFree(visInfo);
		XFree(fbConfigs);
		XCloseDisplay(d);
		d = 0;
   }
}

void mess::process_timers() {
	int T = time1000();
	if (nearest_timer > 0 && T < nearest_timer) return;
	nearest_timer = 0;
	each (i, timers) if (timers[i].on) {
		if (timers[i].next <= T) {
			timers[i].next = time1000() + timers[i].interval;
			timers[i].w->timer(timers[i].id);// remember: timer can be deleted inside the callback
			break;
		}
	}
	for (int i = !timers - 1; i >= 0; i--) {
		if (timers[i].on == false) timers.del(i, 1);
	}
	each (i, timers) {
		if (nearest_timer == 0 || timers[i].next < nearest_timer) nearest_timer = timers[i].next;
	}
}

	char keyz[1024] = {0};
   bool quit = false;
	win *wnd;
	bool physical;
   XEvent event, nev;

bool mess::run() {
	each (i, MESS.all) if (MESS.all[i]->renders > 0) {
		MESS.all[i]->renders = 0;
		MESS.all[i]->render();
	}
   if (!quit) {
		if (!XPending(MESS.d)) return quit;

/*      while (true) {
			if (XPending(MESS.d)) break;
			usleep(1000);
			if (XPending(MESS.d)) break;
			each (i, MESS.all) if (MESS.all[i]->renders > 0) {
				MESS.all[i]->renders = 0;
				MESS.all[i]->render();
			}
			MESS.process_timers();
      }*/
		XNextEvent(MESS.d, &event);

  //		printf("Event: %i\n", event.type);
		switch(event.type) {
      case ConfigureNotify:
			wnd = MESS.handle2win(event.xconfigure.window);
         wnd->x_resize(event.xconfigure.x, event.xconfigure.y, event.xconfigure.width, event.xconfigure.height);
			break;
      case Expose:
			wnd = MESS.handle2win(event.xexpose.window);
			wnd->x_expose();
         break;
      case CirculateNotify:
			break;
		case FocusIn:
			wnd = MESS.handle2win(event.xfocus.window);
			wnd->x_focus(true);
			break;
		case FocusOut:
			wnd = MESS.handle2win(event.xfocus.window);
			wnd->x_focus(false);
		break;
      case MotionNotify:
			while (XCheckWindowEvent(MESS.d, event.xmotion.window, PointerMotionMask, &nev))
				event.xmotion.x = nev.xmotion.x, event.xmotion.y = nev.xmotion.y;
			wnd = MESS.handle2win(event.xmotion.window);
			wnd->cursor(event.xmotion.x, event.xmotion.y);
			break;
      case DestroyNotify:
         quit = true;
			wnd = MESS.handle2win(event.xdestroywindow.window);
			wnd->x_destroy();
         break;
      case GravityNotify:
			printf("gravity: %i\n", event.xgravity.x, event.xgravity.y);
			break;
      case ReparentNotify:
      case MapNotify:
			wnd = MESS.handle2win(event.xmap.window);
			wnd->resetGL();
			wnd->paint();
         break;
      case UnmapNotify:
         break;
		case KeyPress:
				int k;
				k = event.xkey.keycode;
				physical = (keyz[k] == 0);
				keyz[k] = 1;
				if (XFilterEvent(&event, event.xkey.window)) break;//{ continue; }
				wnd = MESS.handle2win(event.xkey.window);
//				if (k == 9 || k == 22 || k == 23 || k == 36 || k == 119) break;
				char buf[20];
				KeySym keysym; Status status;
				int count, W;
				W = 0;
				count = Xutf8LookupString(wnd->ic, (XKeyPressedEvent*)&event, buf, 20, &keysym, &status);
				if (count > 0) {
					str s; s(count); move(buf, *s, count);
					wstr w = utf2w(s);
					W = w[0];
					//printf("%s, w = %i, %s\n", *s, (unsigned int)w[0], *hex_to_str((unsigned int)w[0]));
//					wnd->character(w[0]);
				}
				wnd->keyboard(true, W, k, physical);
			break;
		case KeyRelease:
			// filter out key-repeats
			physical = true;
			if (XPending(MESS.d)) {
				XPeekEvent(MESS.d, &nev);
				if (nev.type == KeyPress && nev.xkey.time == event.xkey.time && nev.xkey.keycode == event.xkey.keycode) {
					physical = false;
				}
			}
			if (physical) keyz[event.xkey.keycode] = 0;
			wnd = MESS.handle2win(event.xkey.window);
			wnd->keyboard(false, 0, event.xkey.keycode, physical);
			break;
		case ButtonPress:
		case ButtonRelease:
			int down, d, B, x, y;
			x = event.xbutton.x, y = event.xbutton.y;
			down = event.type == ButtonPress;
			B = -1;
			d = event.xbutton.button;
			if (d == 4) B = 3, down = false;
			if (d == 5) B = 3;
			if (d == 1) B = 0;
			if (d == 2) B = 2;
			if (d == 3) B = 1;
			if (d == 6) B = 20; // wheel->left/right (what?!)
			if (d == 7) B = 21;
			wnd = MESS.handle2win(event.xbutton.window);
			static int doubleClickTime, doubleClickX, doubleClickY;
			#define doubleClickSetting 300
			if (B == 0 && down) {
				if (doubleClickX == x && doubleClickY == y && time1000() < doubleClickTime + doubleClickSetting) {
					B = 10;
					doubleClickTime = 0;
				} else doubleClickTime = time1000(), doubleClickX = x, doubleClickY = y;
			}
			if (B == 10) wnd->mouse(down, 0, x, y);
			wnd->mouse(down, B, x, y);
			break;
		case KeymapNotify:
			XRefreshKeyboardMapping(&event.xmapping); // what is that?
			break;
		case VisibilityNotify:
			// do something?
			break;
		case EnterNotify:
			break;
		case LeaveNotify:
			break;
      default:
         printf("Type %d\n", event.type);
         break;
      }
   }
   return quit;
}

int add_font(str path, int size) {
	MESS.Fonts++;
	MESS.Fonts[-1].create(path, size);
	return !MESS.Fonts - 1;
}

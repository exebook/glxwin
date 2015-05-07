#define BUILDING_NODE_EXTENSION
/*#include <node.h>
using namespace v8;

#define fassign(name) name = Persistent<Function>::New(Handle<Function>::Cast(Handle<Object>::Cast(a[0])->Get(String::NewSymbol(#name))));

Persistent<Function> paint, print;
Handle<Value> ARGS[10];

Handle<Value> reg(const Arguments& a) {
	HandleScope scope;
	fassign(print)
	fassign(paint)
	return scope.Close(String::New("GLXWIN!"));
}

Handle<Value> run(const Arguments& a) {
	HandleScope scope;
	paint->Call(Context::GetCurrent()->Global(), 0, ARGS);
	return print->Call(Context::GetCurrent()->Global(), 0, ARGS);
	return Undefined();
}

void init(Handle<Object> exports) {
	exports->Set(String::NewSymbol("reg"), FunctionTemplate::New(reg)->GetFunction());
	exports->Set(String::NewSymbol("run"), FunctionTemplate::New(run)->GetFunction());
}

NODE_MODULE(glxwin, init)
*/

#define fassign(name) name = Persistent<Function>::New(Handle<Function>::Cast(Handle<Object>::Cast(a[0])->Get(String::NewSymbol(#name))));

#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include "glxwin.h"

using namespace v8;

//#include "nativeutil.h"

Persistent<Function> onChar, onCursor, onMouse, 
//onPaint, onKey, 
onTimer, onSize, onPipe, onFocus;
Persistent<Object> glxwin, node_exports; // наверное это одно и то же
Persistent<Array> callbacks;

Persistent<Context> context;
Handle<Value> ARGS[10];
double HANDLE(void *x) { return 0 + (long)x; }
void *HANDLE(double x) { return (void*) (long) x; }
void* HANDLE(Local<Value> t) { return HANDLE(Handle<Number>::Cast(t)->Value()); }
double NUMBER(Local<Value> t) { return Handle<Number>::Cast(t)->Value(); }
wstr WSTR(Local<Value> t) {
	String::Value s(t);
	wstr w; w(s.length()); if (!w) move(*s, *w, w.occu());
	return w;
}


#define function(name) Handle<Value> name(const Arguments& a)
#include "v8util.h"
#include "v8file.h"
#include "shellpipe.h"
//double main_window_handle = 0;
namespace unixstuff {
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <termios.h>
#include <pty.h>
#include <stdlib.h>

#include <sys/types.h>
}

Handle<Value> js_call(Handle<Function> f, int arg_count) {
	TryCatch t;
	Handle<Value> R = f->Call(glxwin, arg_count, ARGS);
	if (R.IsEmpty()) {
		printf("%s\n", *v8_error_as_string(& t).compat());
//		Result->Set(String::New("success"), Boolean::New(false));
//		Handle<Object> E = v8_error_as_object(&try_catch);
//		E->Set(String::New("source"), source);
//		Result->Set(String::New("error"), E);
//		return handle_scope.Close(Result);
	}
	return R;
}

struct inherit(v8win, win) {
	int font_color;
	v8win() : font_color(0) {
	}
	~v8win() {
	}
	on_created {
//		add_timer(1000, 10);
	}

	on_timer {
		if (id == 1000) {
//			shell_timer();
			return;
		}
		HandleScope handle_scope;
		ARGS[0] = Number::New(HANDLE(this));
		ARGS[1] = Number::New(id);
		js_call(onTimer, 2);
	}
	on_key {
		HandleScope handle_scope;
		Handle<Object> B = Object::New();
		B->Set(String::New("call"), String::New("onKey"));
		B->Set(String::New("handle"), Number::New(HANDLE(this)));
		B->Set(String::New("down"), Boolean::New(down));
		if (charcode != 0) B->Set(String::New("char"), String::New((uint16_t*)&charcode, 1));
		B->Set(String::New("key"), Integer::New(key));
		B->Set(String::New("physical"), Boolean::New(physical));
//		Context::GetCurrent()->Global()->Set(String::New("glxwin_key"), B);
		int len = callbacks->Length();
		callbacks->Set(len, B);
	}
	on_paint {
		Handle<Object> B = Object::New();
		B->Set(String::New("call"), String::New("onPaint"));
		B->Set(String::New("handle"), Number::New(HANDLE(this)));
		int len = callbacks->Length();
		callbacks->Set(len, B);
	}

	on_mouse {
		HandleScope handle_scope;
		ARGS[0] = Number::New(HANDLE(this));
		ARGS[1] = Number::New(button);
		ARGS[2] = Number::New(down);
		ARGS[3] = Number::New(x);
		ARGS[4] = Number::New(y);
		js_call(onMouse, 5);
	}
	on_size {
		HandleScope handle_scope;
		ARGS[0] = Number::New(HANDLE(this));
		ARGS[1] = Number::New(w);
		ARGS[2] = Number::New(h);
		js_call(onSize, 3);
	}
	on_cursor {
		HandleScope handle_scope;
		ARGS[0] = Number::New(HANDLE(this));
		ARGS[1] = Number::New(x);
		ARGS[2] = Number::New(y);
		js_call(onCursor, 3);
	}
	on_focus {
		HandleScope handle_scope;
		ARGS[0] = Number::New(HANDLE(this));
		ARGS[1] = Boolean::New(on);
		js_call(onFocus, 2);
	}
};

//#include "termwin.h"

/*function(print) {
	HandleScope handle_scope;
	wstr w = WSTR(a[0]);
	printf(*w2utf(w));
	return Undefined();
}*/
wstr invoke(str cmd) {
	wstr R;
	FILE* pipe = popen(*cmd, "r" );
	if (pipe == NULL ) {
		return (wstr)"native_sh(" + cmd + ") failed";
	}
	char buf[128];
	while (!feof(pipe)) {
		if (fgets(buf, 128, pipe)) {
			str s = buf;
			R / utf2w(s);
		}
	}
	pclose(pipe);
	return R;
}

function (native_sh) {
	HandleScope handle_scope;
	wstr txt = WSTR(a[0]);
	wstr R = invoke(w2utf(txt));
	return String::New((uint16_t*)*R);
}

//function (native_exec) {
//	HandleScope handle_scope;
//	wstr cmd = WSTR(a[0]);
//	using namespace unixstuff;
//	if (fork() == 0) {
//		//while (true)
//		fcntl(XConnectionNumber(MESS.d), F_SETFD, fcntl(XConnectionNumber(MESS.d), F_GETFD) | FD_CLOEXEC);
//		wstr R = invoke(cmd);
//		exit(0);
//	}
////	system(*w2utf(cmd));
//	return Undefined();
//}

function (create_win) {
	HandleScope handle_scope;
	v8win *parent = (v8win*) HANDLE(a[0]);
	v8win *W = new v8win;
	wstr fontName = "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf";
	int fontSize = 18;
	if (a[1] != Undefined()) fontName = WSTR(a[1]);
	if (a[2] != Undefined()) fontSize = NUMBER(a[2]);
	W->create();
	W->curfont = add_font(fontName, fontSize);
	if (a[3] != Undefined()) MESS.Fonts[W->curfont].extra_inter_char = NUMBER(a[3]);
	if (a[4] != Undefined()) MESS.Fonts[W->curfont].tune_y = NUMBER(a[4]);
//	if (main_window_handle == 0) main_window_handle = HANDLE(W);
	return Number::New(HANDLE(W));
}

function (step) {
	HandleScope handle_scope;
	bool result = MESS.step();
	return Boolean::New(result);
}

function (step_renders) {
	HandleScope handle_scope;
	MESS.step_renders();
	return Undefined();
}

function (show) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	W->show();
	return Undefined();
}

function (hide) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	W->hide();
	return Undefined();
}

function (repaint) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	W->paint();
	return Undefined();
}

function (force_repaint) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	W->render();
	return Undefined();
}

function (print) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	wstr txt = WSTR(a[1]);
	int x = NUMBER(a[2]);
	int y = NUMBER(a[3]);
	glColor4ubv((GLubyte*)&W->font_color);
	W->print(txt, x, y);
	return Undefined();
}

function (text_extent) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	wstr txt = WSTR(a[1]);
	int *N = W->text_extent(txt);
	Handle<Array> A = Array::New(2);
	A->Set(0, Integer::New(N[0]));
	A->Set(1, Integer::New(N[1]));
	return handle_scope.Close(A);
}

function (crect) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	int x = NUMBER(a[1]);
	int y = NUMBER(a[2]);
	int x1 = NUMBER(a[3]);
	int y1 = NUMBER(a[4]);
	unsigned int color = NUMBER(a[5]);
//	color |= 0xff000000;
	W->crect(x, y, x1, y1, color);
	return Undefined();
}
/*
function(native_add_timer) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	int id = NUMBER(a[1]);
	int T = NUMBER(a[2]);
	W->add_timer(id, T);
	return Undefined();
}

function(native_del_timer) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	int id = NUMBER(a[1]);
	W->del_timer(id);
	return Undefined();
}
*/
function(set_xywh) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	if (a[1] != Undefined()) {
		double x = NUMBER(a[1]);
		double y = NUMBER(a[2]);
		W->move(x, y);
	}
	if (a[3] != Undefined()) {
		double w = NUMBER(a[3]);
		double h = NUMBER(a[4]);
		W->size(w, h);
	}
	return Undefined();
}

function(get_xywh) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	Handle<Array> A = Array::New(4);
	A->Set(0, Integer::New(W->x));// TODO: fix this
	A->Set(1, Integer::New(W->y));
	A->Set(2, Integer::New(W->w));
	A->Set(3, Integer::New(W->h));
	return handle_scope.Close(A);
}

function(font_color) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	unsigned int color = NUMBER(a[1]);
	W->font_color = color | 0xff000000;
	return Undefined();
}
/*
function (yaui_platform) {
	HandleScope handle_scope;
	return String::New("linux");
}
*/
function(apply_font) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	wstr name = WSTR(a[1]);
	int size = NUMBER(a[2]);
	int color = NUMBER(a[3]);
	int bgcolor = NUMBER(a[4]);
//	int font_id = W->S->apply_font(name, size, color);
	Handle<Array> A = Array::New(3);
	int *N = W->text_extent('A');
	A->Set(0, Integer::New(N[0]));
	A->Set(1, Integer::New(N[1]));
	A->Set(2, Integer::New(W->curfont));
	return handle_scope.Close(A);
}

/*function(main_handle) {
	HandleScope handle_scope;
	return Number::New(main_window_handle);
}*/

function(get_xwindow_handle) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	return Number::New((int)(W->window));
}

function (color_text) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	yaglfont::Font *f = & MESS.Fonts[W->curfont];
	int x = NUMBER(a[1]);
	int y = NUMBER(a[2]);
	int w = NUMBER(a[3]);
	int h = NUMBER(a[4]);
	glLoadIdentity();
	glTranslatef(x, y, 0);
	Handle<Array> TEXTS = Handle<Array>::Cast(a[5]);
	Handle<Array> COLORS = Handle<Array>::Cast(a[6]);
	int n = 0;
	wstr s; s(w); unsigned int *BITS = new unsigned int [w];
	for (int Y = 0; Y < h; Y++) {
		Handle<Array> TEXT = Handle<Array>::Cast(TEXTS->Get(Y));
		Handle<Array> COLOR = Handle<Array>::Cast(COLORS->Get(Y));
		for (int X = 0; X < w; X++) {
			uint32_t CHAR = (uint32_t) Handle<Number>::Cast(TEXT->Get(X))->Value();
			uint32_t CLR = (uint32_t) Handle<Number>::Cast(COLOR->Get(X))->Value();
			s[X] = CHAR; BITS[X] = CLR;
		}
		draw_line(*f, s, BITS);
		glTranslatef(0, f->metric.h, 0);
	}
	delete[] BITS;
	return Undefined();
}

function (color_text_new) {
	// this was created to replace color_text
	// Arrayof {} was very slow compared
	// to typed array/buffer
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	yaglfont::Font *f = & MESS.Fonts[W->curfont];
	int x = NUMBER(a[1]);
	int y = NUMBER(a[2]);
	int w = NUMBER(a[3]);
	int h = NUMBER(a[4]);
	glLoadIdentity();
	glTranslatef(x, y, 0);
	word *T = (word*)node::Buffer::Data(a[5]);
	u32 *C = (u32*)node::Buffer::Data(a[6]);
	u32 *CLR = C;
	int n = 0;
	wstr s;
	s.p->size = w;
	s.p->p = (wchar_t*)T;
	for (int Y = 0; Y < h; Y++) {
		
		draw_line(*f, s, CLR);
		glTranslatef(0, f->metric.h, 0);
		CLR += w;
		s.p->p += w;
	}
	s.p->p = 0; s.p->size = 0;
	return Undefined();
}

function (paintBegin) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	glXMakeCurrent(MESS.d, W->window, W->glx);
	glLoadIdentity();
	glColor4f(0, 0, 0, 1);
	return Undefined();
}

function (paintEnd) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	W->flush();
	return Undefined();
}

/*function (native_hide_all_children) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	XUnmapSubwindows(MESS.d, W->window);
	return Undefined();
}

function (native_show_all_children) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	XMapSubwindows(MESS.d, W->window);
	return Undefined();
}
*/
function (native_isdir) {
	HandleScope handle_scope;
	using namespace unixstuff;
	struct stat t;
	str path = w2utf(WSTR(a[0]));
	stat(*path, &t);
	return Boolean::New(S_ISDIR(t.st_mode));
}

Handle<Array> convert_time(tm *m) {
	using namespace unixstuff;
	Handle<Array> C = Array::New(6);
	int j = 0;
	C->Set(j++, Integer::New(m->tm_year+1900));
	C->Set(j++, Integer::New(m->tm_mon + 1));
	C->Set(j++, Integer::New(m->tm_mday));
	C->Set(j++, Integer::New(m->tm_hour));
	C->Set(j++, Integer::New(m->tm_min));
	C->Set(j++, Integer::New(m->tm_sec));
	return C;
}
function (native_readdir) {
	HandleScope handle_scope;
	using namespace unixstuff;
	DIR *dp;
	dirent *dirp; struct stat t;
	str path = w2utf(WSTR(a[0]));
	if ((dp  = opendir(*path)) == NULL) {
		Handle<Array> A = Array::New(0);
		Handle<Object> B = Object::New();
		int j = 0;
		B->Set(String::New("name"), String::New(strerror(errno)));
		B->Set(String::New("hint"), Boolean::New(true));
		A->Set(0, B);
		return handle_scope.Close(A);
	}
	Handle<Array> A = Array::New(0);
	int i = 0;
	if (path[-1] != '/') path/'/';
	while ((dirp = readdir(dp)) != NULL) {
		str s = dirp->d_name;
		if (s == "." || s == "..") continue;
		str f = path + s;
		stat(*f, &t);
		str p = "----------"; int pp = t.st_mode;
		if ((pp & 0400) != 0) p[0] = 'r';
		if ((pp & 0200) != 0) p[1] = 'w';
		if ((pp & 0100) != 0) p[2] = 'x';
		if ((pp & 0040) != 0) p[3] = 'r';
		if ((pp & 0020) != 0) p[4] = 'w';
		if ((pp & 0010) != 0) p[5] = 'x';
		if ((pp & 0004) != 0) p[6] = 'r';
		if ((pp & 0002) != 0) p[7] = 'w';
		if ((pp & 0001) != 0) p[8] = 'x';
		Handle<Object> B = Object::New();
		B->Set(String::New("name"), String::New(dirp->d_name));
		B->Set(String::New("size"), Number::New(t.st_size));
		B->Set(String::New("mode"), Integer::New(t.st_mode & 0xfff));
		B->Set(String::New("flags"), String::New(*p, !p));
		B->Set(String::New("dir"), Boolean::New(S_ISDIR(t.st_mode)));
		B->Set(String::New("ctime"), convert_time(localtime(&t.st_ctime)));
		B->Set(String::New("mtime"), convert_time(localtime(&t.st_mtime)));
		B->Set(String::New("atime"), convert_time(localtime(&t.st_atime)));
		A->Set(i++, B);
	}
	closedir(dp);
	return handle_scope.Close(A);
}

function(register_callbacks) {
	glxwin = Persistent<Object>::New(Handle<Object>::Cast(a[0]));
	HandleScope scope;
//	fassign(onPaint);
	fassign(onCursor);
	fassign(onMouse);
//	fassign(onPaint);
//	fassign(onKey);
	fassign(onTimer);
	fassign(onChar);
	fassign(onSize);
	fassign(onPipe);
	fassign(onFocus);
	return scope.Close(Undefined());
}

Handle<Value> Method(const Arguments& args) {
  HandleScope scope;
  return scope.Close(String::New("GLXWIN!"));
}

function(setCursor) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	int cursor = NUMBER(a[1]);
	W->setCursor(cursor);
	return Undefined();
}

function (setSizeSteps) {
	HandleScope handle_scope;
	v8win *W = (v8win*) HANDLE(a[0]);
	int w = NUMBER(a[1]);
	int h = NUMBER(a[2]);
	W->setSizeSteps(w, h);
	return Undefined();
}

function (x11quit) {
	return Boolean::New(MESS.quit);
}

void init(Handle<Object> exports) {
	node_exports = Persistent<Object>::New(Handle<Object>::Cast(exports));
	exports->Set(String::NewSymbol("hello"), FunctionTemplate::New(Method)->GetFunction());
	callbacks = Persistent<Array>::New(Handle<Array>::Cast(Array::New(0)));
	exports->Set(String::New("c++callbacks"), callbacks);

//      context->Global()
	#define function(name) exports->Set(String::NewSymbol(#name), FunctionTemplate::New(name)->GetFunction());
	function(register_callbacks)
	function(native_sh)
	function(native_isdir)
	function(native_readdir)
	function(native_copy_start)
	function(native_copy_next)
	function(v8_exit)

	function(get_xwindow_handle)
	function(create_win)
	function(show)
	function(hide)
	function(step)
	function(step_renders)
	function(x11quit)
	function(print)
	function(repaint)
	function(force_repaint)
	function(text_extent)
	function(crect)
	function(set_xywh)
	function(get_xywh)
	function(font_color)
	function(apply_font)
	function(color_text)
	function(color_text_new)
	function (paintBegin)
	function (paintEnd)
	function (setCursor)
	function (setSizeSteps)
}

NODE_MODULE(glxwin, init)

/*


//	function(native_hide_all_children)
//	function(native_show_all_children)
//	function(native_open_xterm)
//	function(native_exec_xterm)

	start_shell(110);

//	function(native_glyph)
//	function(native_xhandle)
	#undef function
	context = Context::New(0, G);
	Context::Scope context_scope(context);
	bind_functions();
	str bootstrap = "bootstrap.js";
	if (!fileexists(*bootstrap)) bootstrap = exe_dir("../bootstrap.js");
	if (!fileexists(*bootstrap)) bootstrap = "/usr/lib/deodar/bootstrap.js";
	wstr r;
	try {
		r = v8_exe(bootstrap, bootstrap, true);
		if (v8_exe_error) printf("ERROR: '%s'\n", *r.compat());
	} catch (int e) {
		printf("Yaui.cpp:%i unhandled exception, terminating\n", __LINE__);
	}

	Handle<Value> args[2];
	args[0] = Number::New(0);
	args[1] = Number::New(0);
//	Handle<Value> js_result = yaui_main->Call(context->Global(), 2, args);
	return 0;
}
*/

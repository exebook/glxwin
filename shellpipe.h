/*#include <errno.h>

int fd[2], back[2]; int amaster; int shell_pid = -1; bool shell_working = false;

void write_pipe(int f, str s) {
	using namespace unixstuff;
	uint32_t X = !s;
	write(f, &X, 4);
	write(f, *s, !s);
}

bool have_data(int f, int time) {
	fd_set set;
	struct timeval timeout;
	FD_ZERO(&set);
	FD_SET(f, &set);
	timeout.tv_sec = 0; timeout.tv_usec = time;
	return select(FD_SETSIZE, &set, NULL, NULL, &timeout);
}

str read_pipe(int f) {
	using namespace unixstuff;
	uint32_t X = 0;
	while (true) {
		if (have_data(f, 10)) break;
	}
	int size = read(f, &X, 4);
	str R; R(X);
	read(f, *R, !R);
	return R;
}

void loop_shell(int columns) {
	using namespace unixstuff;
	system("stty sane");
	system("stty 115200");
	str s = "stty columns "; s / columns;
	system(*s);
	//stty -olcuc // disable caps
	while (true) {
		str cmd = read_pipe(fd[0]);
		if (cmd.pos("cd ") == 0) {
			bite(cmd, " ");
			chdir(*cmd);
		} else
		system(*cmd);
		write_pipe(back[1], "done");
	}
}

void start_shell(int columns) {
	using namespace unixstuff;
	pipe(fd);
	pipe(back);
	termios termp; winsize winp;
	char name[128];
	int pid = forkpty(&amaster, name, &termp, &winp);
	if (pid == 0) {
		close(fd[1]);
		close(back[0]);
		loop_shell(columns);
	}
	shell_pid = pid;
	close(fd[0]);
	close(back[1]);
	shell_working = false;
}

void read_amaster() {
	using namespace unixstuff;
	const int buff_size = 1024;
	char buf[buff_size]; int size;
	while (have_data(amaster, 10)) {
		size = read(amaster, buf, buff_size - 1);
		if (size <= 0) break;
		buf[size] = 0;
		wstr w = utf2w(buf);
		ARGS[0] = String::New((uint16_t*)*w, !w);
		bool B = Handle<Number>::Cast(js_call(onPipe, 1))->Value();
	}
}

void command_shell_sync(str s) {
	using namespace unixstuff;
	shell_working = true;
	write_pipe(fd[1], s);
	while (true) {
		if (have_data(back[0], 10)) {
			str done = read_pipe(back[0]);
			read_amaster();
			break;
		}
		read_amaster();
	}
	shell_working = false;
}

void command_shell_async(str s) {
	using namespace unixstuff;
	shell_working = true;
	write_pipe(fd[1], s);
}

void shell_timer() {
	if (have_data(back[0], 1)) {
		str done = read_pipe(back[0]);
		read_amaster();
		shell_working = false;
		ARGS[0] = Undefined();
		js_call(onPipe, 1);
		return;
	}
	read_amaster();
}

function (native_sh_signal) {
	HandleScope handle_scope;
	using namespace unixstuff;
	kill(shell_pid, SIGKILL);
	close(fd[1]);
	close(back[0]);
	int columns = NUMBER(a[0]);
	start_shell(columns);
	return Undefined();
}

function (native_sh_async) {
	HandleScope handle_scope;
	if (shell_working) return Undefined();
//	callback = Handle<Function>::Cast(a[0]);
	wstr cmd = WSTR(a[0]);
	if (a[1] != Undefined()) {
		wstr dir = WSTR(a[1]);
		str s = w2utf(dir);
		command_shell_sync((str)"cd " + dir);
		//unixstuff::chdir(*s);
	}
	command_shell_async(w2utf(cmd));
	return Boolean::New(true);
}

function (native_sh_write) {
	using namespace unixstuff;
	HandleScope handle_scope;
	wstr w = WSTR(a[0]);
	str s = w2utf(w);
	write(amaster, *s, !s);
	return Boolean::New(true);
}
*/
int copy_in, copy_out, copy_size, copy_done, copy_count = 0;

function (native_copy_start) {
	using namespace unixstuff;
	HandleScope handle_scope;
	str source = w2utf(WSTR(a[0])), dest = w2utf(WSTR(a[1]));

	struct stat t;
	stat(*source, &t);
	copy_size = t.st_size; copy_done = 0;

	copy_in = open(*source, O_RDONLY);
	if (copy_in < 0) return Number::New(-1);
	copy_count++;
	copy_out = open(*dest, O_WRONLY|O_CREAT|O_TRUNC, 0777);
	if (copy_out < 0) { close(copy_in); return Number::New(-2); }
	copy_count++;
	return Number::New(copy_size);
}

function (native_copy_next) {
	using namespace unixstuff;
	HandleScope handle_scope;
	bool proceed = NUMBER(a[0]);
	if (!proceed) {
		close(copy_in); close(copy_out);
		return Number::New(-1);
	}
	const int BUFSIZE = 8192;
	char buf[BUFSIZE];
	int blocks = 10;
	if (a[1] != Undefined()) blocks = NUMBER(a[1]);
	while (blocks-- > 0) {
		ssize_t result = read(copy_in, &buf[0], BUFSIZE);
		copy_done += result;
		bool done = false;
		if (result <= 0) done = true; else done = write(copy_out, &buf[0], result) != result;
		if (copy_done >= copy_size) done = true;
		if (done) {
			close(copy_in); close(copy_out);
			copy_count -= 2;
			return Number::New(-1);
		}
		if (result < BUFSIZE) break;
	}
	return Number::New(copy_done);
}


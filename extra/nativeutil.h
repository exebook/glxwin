
/*
char* getexename() {
	static char buf[256] = {0}; size_t size = 256;
	if (buf[0]) return buf;
	using namespace unixstuff;
    char linkname[64];
    pid_t pid; int ret;
    pid = getpid();
    if (snprintf(linkname, sizeof(linkname), "/proc/%i/exe", pid) < 0) abort();
    ret = readlink(linkname, buf, size);
    if (ret == -1) return NULL;
    if (ret >= size) { errno = ERANGE; return NULL; }
    buf[ret] = 0;
    printf("processing\n");
    return buf;
}

str _exe_dir(str s) {
	static str r = getexename();
	while (r[-1] != '/') r(!r - 1);
	return r + s;
}

*/

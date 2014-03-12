{
  "targets": [
    {
	"target_name": "glxwin",
	"include_dirs": ["/usr/include/freetype2", "osnova", "extra"],
	"sources": [ "glxwin.cpp" ],
	"cflags": ["-O0 -w -Wfatal-errors -fshort-wchar -g"],
	"defines": ["UNIX"],
	"libraries": ["-lfreetype", "-lv8", "-lX11", "-lGL", "-lutil"]
    }
  ],"variables" : { "clang" : 1 }
}

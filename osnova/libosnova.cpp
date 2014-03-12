# ifndef osnova_lib
# define osnova_lib
# ifdef __BORLANDC__
# ifndef LINUX
#   define WIN32
# endif // linux
# endif // __BORLANDC__
#undef ABS

//# include <lc.h>
#ifdef _MSC_VER
#  if (_MSC_VER <= 1200)
#    define for if(0);else for
#  endif
#endif

# define  self (*this)
# define  loop while(true)
# define  each_slow(a, b) for (int a = 0; a < !b; a++)
# define  each(a, b) for (int a = 0, each_counter = !b; a < each_counter; a++)
# define  each_down(a, b) for (int a = !b - 1; a >= 0; a--)
# define  each2(a, b) for (a = 0; a < !b; a++)
# define  mean(a)
# define crash ((char *)0)[0] = 0;
# ifndef  isalpha
#   define  isalpha(ch) ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_'))
#   define  isdigit(ch) (ch >= '0' && ch <= '9')
# endif
# define unconst(t, v) * (t *) (void *) & v
# define dllex(a) extern "C" a __declspec(dllexport)
//# define sub_int(a)struct a{int i;a(){};a(int c){i=c;} operator int(int){return i;}void operator=(int c){i=c;}};
#define inherit(b, a) \
  child_##b##_##p : a { /*private:*/ typedef a inherited; };\
  struct b : child_##b##_##p
# define startup(N) \
  void S_##N##_C(); \
  namespace S_##N { struct i { i() { S_##N##_C(); } } I; } void S_##N##_C()

typedef unsigned short word;
#ifdef UNIX
//#define wchar_t word
#endif
typedef char i8;
typedef short i16;
typedef int i32;
typedef unsigned char byte;
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;
typedef float f32;
typedef double f64;

#ifdef UNDER_CE
#define type64 __int64
#  else
#    ifdef _MSC_VER
#       define type64 __int64
#    else
#       ifdef DOS
#          define type64
#       else
#          define type64 long long
#       endif
#    endif
#endif
typedef type64 i64;
typedef unsigned type64 u64;

#ifdef UNIX
namespace unixstuff
{
/*
FREEBSD OK
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/time.h>
#include "arpa/inet.h"
#include <pwd.h>
#include <X11/Xauth.h>
*/
//#ifdef LINUX
//#include <unistd.h>
//#include <linux/un.h>
//#endif
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
//#include <unistd.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/time.h>
#include "arpa/inet.h"
#include <pwd.h>
#ifdef XWIN
#include <X11/Xauth.h>
#endif
//#define wchar_t unsigned short
}

#endif // UNIX

# include "basic.cpp"
# include "charutil.cpp"
# include <low_api.l>
# ifndef MINIOSNOVA
# include "strings.cpp"
# include "strutil.cpp"
# include "list.h"
# include "arr.h"
# include "join.cpp"
//# include "cmdline.h"
# endif
# endif // osnova_lib



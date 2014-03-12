# ifndef strings_cpp
# define strings_cpp

# include "bootstr.cpp"
struct str;
# define onebyte
# define me str
# define bit char
# define bytesize(a) a
#   include "str.cpp"
# undef me
# undef bit
# undef onebyte
# undef bytesize

#ifndef NO_WSTR
# define me wstr
# define bit wchar_t
# define bytesize(a) (a * 2)
#   include "str.cpp"
# undef me
# undef bit
#endif // NO_WSTR

# ifdef UNICODE
  typedef wstr Str;
  typedef wchar_t Char;
# else
  typedef str Str;
  typedef char Char;
# endif // UNICODE

str load(str name)
{
   str R;
   R(filesize(*name));
   if (!R > 0)
     load(*name, *R, !R);
   return R;
}

#ifndef NO_WSTR
wstr wload(str name)
{
   str s = load(name);
   wstr R;
   R.binary(s);
   if (!R > 2) 
   if (R[0] == 0xfeff) R.del(0, 1);
   return R;
}

void wtrim(wstr &w)
{
   int a = 0, b = 0;
   each (x, w) if (w[x] == ' ') a++; else break;
   for (int x1 = !w - 1; x1 > a; x1--)
     if (w[x1] == ' ') b++; else break;
   int l = !w - a - b;
   move(& w[a], *w, l * 2);
   w(l);
}

int wstr_cmp(wstr &A, wstr &B)// 1: a<b   -1: a>b
{
   if (!A == 0) return 2;
   if (!B == 0) return 3;
   wchar_t *a = *A, *b = *B, *alast = &A.p->p[!A - 1], *blast = &B.p->p[!B - 1];
   while (true)
   {
      if (a > alast)
      {
         if (b > blast) return 0;
         return 1;
      }
      if (b > blast) return -1;
      if (*a < *b) return 1;
      if (*a > *b) return -1;
      a++; b++;
   }
}

#endif // NO_WSTR
template <typename T>void trim(T &s)
{
   if (!s == 0) return;
   int a = 0, b = 0;
   each (x, s)
     if (s[x] == ' ' && s[x] > 5) a++; else break;
   if (a == !s)
   {
      s(0);
      return;
   }
   for (int x1 = !s - 1; x1 > a; x1--)
     if (s[x1] <= ' ' && s[x1] > 5)
       b++;
         else break;
 //  int l = !s - a - b;
   s.del(0, a);
   s(!s - b);
}

# endif // strings_cpp

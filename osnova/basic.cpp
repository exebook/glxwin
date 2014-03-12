#  define Ob8(a) \
        (a         & 1) \
    | (((a >> 3)  & 1) << 1) \
    | (((a >> 6)  & 1) << 2) \
    | (((a >> 9)  & 1) << 3) \
    | (((a >> 12) & 1) << 4) \
    | (((a >> 15) & 1) << 5) \
    | (((a >> 18) & 1) << 6) \
    | (((a >> 21) & 1) << 7) \
    | (((a >> 24) & 1) << 8) \
    | (((a >> 27) & 1) << 9) \
    | (((a >> 30) & 1) << 10) \
    | (((a >> 33) & 1) << 11) \
    | (((a >> 36) & 1) << 12) \
    | (((a >> 39) & 1) << 13) \
    | (((a >> 42) & 1) << 14) \
    | (((a >> 45) & 1) << 15)
#
#ifdef UNDER_CE
#  define _B(a) Ob8((unsigned long)a)
#else
#  define _B(a) Ob8((unsigned long long)a)
#endif
#
# define mmove(a, b, c) \
    move( (void *) (a), (void *) (b), (int) c)

void move(void *src, void *dest, int count)
{
  if (count <= 0) return;
  char *a = (char *)src; char *b = (char *)dest;
  while (count--) *b++ = *a++;
}

void movew(void *src, void *dest, int count)
{
   wchar_t *b = (wchar_t *)src, *a = (wchar_t *)dest;
   if (count > 0) while (count--) *a++ = *b++;
}

void move2(void* from1, void *to1, int count)
{
  if (count <= 0) return;
  char *from = (char*)from1, *to = (char*)to1;
  switch (count % 8)  /* count > 0 assumed */
  {
    case 0:        do {  *to++ = *from++;
    case 7:              *to++ = *from++;
    case 6:              *to++ = *from++;
    case 5:              *to++ = *from++;
    case 4:              *to++ = *from++;
    case 3:              *to++ = *from++;
    case 2:              *to++ = *from++;
    case 1:              *to++ = *from++;
                      } while ((count -= 8) > 0);
  }
}

void move3(void* from1, void *to1, int count)
{
  if (count <= 0) return;
  char *from = (char*)from1, *to = (char*)to1;
  switch (count % 8)  /* count > 0 assumed */
  {
    case 0:        do {  *to++ = *from++;
    case 7:              *to++ = *from++;
    case 6:              *to++ = *from++;
    case 5:              *to++ = *from++;
    case 4:              *to++ = *from++;
    case 3:              *to++ = *from++;
    case 2:              *to++ = *from++;
    case 1:              *to++ = *from++;
                      } while ((count -= 8) > 0);
  }
}

//void move1(void *src, void *dest, int count)
//{
//  if (count <= 0) return;
  //memcpy(dest, src, count);
//}

//# define move(a, b, c) memcpy (b, a, c)

template <typename T> void swap(T &a, T &b) { T t = a; a = b; b = t; }

bool same(void *a, void *b, int size)
{
   if (size == 0) return true;
   if (size < 0) return false;
   char *A = (char *)a, *B = (char *)b;
   while (size--)
   {
      if (*A++ != *B++ ) return false;
   }
   return true;
}

int min(int a, int b)
{
  if (a > b) return b;
  return a;
}

int max(int a, int b)
{
  if (a < b) return b;
  return a;
}
template <typename T> T ABS(T a)
{
  return ((a) >= 0 ? (a) : -(a));
}

template <typename T> T abs(T a)
{
  return ((a) >= 0 ? (a) : -(a));
}

void move32(void *src, void *dest, int count)
{
  int *a = (int*)src; int *b = (int*)dest;
  while (count--) *b++ = *a++;
}

void fill32(int value, int *dest, int count)
{
  int *b = dest;
  while (count--) *b++ = value;
}

void invert32(int *dest, int count)
{
  int *b = dest;
  while (count--)
  {
    *b = !*b;
    b++;
  }
}

void * my_cast(...)
{
   int *n;
   return (int*)(&n)[3];
}

void xchg(void *a, void *b, int size)
{
  char ch;
  char *A = (char *)a, *B = (char *)b;
  while (size-- > 0)
  {
     ch = *A; *A++ = *B; *B++ = ch;
  }
}

void __limit(int &i, int min, int max)
{
 if (min < max)
  {
   if (i < min) i = min;
   else if (i > max) i = max;
  }
 else
  {
   if (i < max) i = max;
   else if (i > min) i = min;
  }
}

//#define var (void *)&

void fillchar(void *dest, int count, char ch)
{
  if (count <= 0) return;
  char *b = (char *)dest;
  while (count--) *b++ = ch;
}

void memset(void *dest, char ch, int count)
{
   fillchar(dest, count, ch);
}

template <typename T>
struct mb
{
   T *a;
   mb& operator () (T &A)  { a = &A; return *this; }
   mb& operator > (T b)    { if (*a <= b) *a = b + 1; return *this; }
   mb& operator < (T b)    { if (*a >= b) *a = b - 1; return *this; }
   mb& operator >= (T b)   { if (*a < b) *a = b; return *this; }
   mb& operator <= (T b)   { if (*a > b) *a = b; return *this; }
};

mb <int> mustbe;
mb <double> dmustbe;

template <typename T>
struct typish
{
   struct N
   {
      char *x;
      T & operator [] (int i)
      {
         return *(T *) & x[i];
      }
   };
   N operator [] (void *c)
   {
      N R;
      R.x = (char *) c;
      return R;
   }
};

// doublish[dd][23]
typish <char> charish;
typish <word> wordish;
typish <int> intish;
typish <float> floatish;
typish <double> doublish;

struct rgb
{
   byte r, g, b, k;
   rgb()
   {
       *(int*)(&r) = 0;
   }

   void operator = (rgb &x)
   {
      intish[&r][0] = intish[&x.r][0];
   }

   rgb(int c)
   {
       * (int *)(& r) = c;
   }

   void operator = (int i)
   {
       *(int *)(& r) = i;
   }

   operator int()
   {
       return * (int *)(& r);
   }
};

#ifdef WIN32
#ifndef _WINBASE_
extern "C"
{
   unsigned long __stdcall GetTickCount();
   int __stdcall Sleep(int);
   void __stdcall QueryPerformanceCounter(void*);
   int __stdcall  QueryPerformanceFrequency(void *);
}
#endif // _WINBASE_

int ticks_freq()
{
   u64 T;
   QueryPerformanceFrequency(&T);
   return T;
}

u64 fast_tick() // 1 sec = 1,000,000
{
  static u64 u, f = (u64)-1;
 #ifdef WIN32
  QueryPerformanceCounter(&u);
  if (f == (u64)-1)
  {
    QueryPerformanceFrequency(&f);
    f /= 1000;
    f /= 1000;
  }
 #endif
  return u / f;
}

int time1000()
{
   //return GetTickCount();
   return fast_tick() / 1000;
}

#else

struct timeval1 {
   int tv_sec;
   int tv_usec;
};
//extern "C" int gettimeofday1(void *, void *);

long time1000()
{
  using namespace unixstuff;
  timeval val;
  gettimeofday(&val, 0);
  val.tv_sec &= 0xffff;
  return val.tv_sec * 1000 + val.tv_usec / 1000;
//  return ticks() / 1000; // na samom dele nado delitj na chastotu...
}
#endif

int daysec = 60 * 60 * 24;

#ifdef UNIX
extern "C" int usleep(int);
#endif

void delay(int seconds, int miliseconds)
{
  #ifdef WIN32
  Sleep(seconds * 1000 + miliseconds);
  #else
  #ifdef UNIX
  int total, current, left, stop;
  total = seconds * 1000 + miliseconds;
  stop = time1000() + total;
  current = time1000();
  while (true) {
   left = stop - current;
   usleep(left * 1000);
   current = time1000();
   if (current > stop) break;
  }
  #endif
  #endif
}

struct rect
{
    int x, y, w, h;
};

int blend(int C1, int C2, int frame, int frame_count)
{
   rgb a = C1, b = C2;
   #define z(x) a.x += ((double)(b.x - a.x) / frame_count) * frame;
   z(r) z(g) z(b)
   #undef z
   return a;
}

int brightness(int c, int x)
{
   int r = c & 0xff, g = (c >> 8) & 0xff, b = (c >> 16) & 0xff;
   r += x; g += x; b += x;
   if (r < 0) r = 0;
   if (g < 0) g = 0;
   if (b < 0) b = 0;
   if (r > 255) r = 255;
   if (g > 255) g = 255;
   if (b > 255) b = 255;
   int R = 0;
   R += r;
   R |= g << 8;
   R |= b << 16;
   return R;
}

//RtlActivateActivationContextUnsafeFast

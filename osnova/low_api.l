#ifndef low_api_l
#define low_api_l

# ifndef UNDER_CE
  byte ticks_bits[3] = { 0x0F, 0x31, 0xC3};
  u64 ( * ticks ) () = (u64 (*) ()) & ticks_bits;
# endif
#ifdef WIN32
//C:\com\VS6\CL.EXE low_api.cpp /link /nodefaultlib:libc /nodefaultlib:oldnames.lib C:\com\VS6\kernel32.lib
#define APP void * __stdcall
#define API int __stdcall

extern "C"
{
   void printf(char *, ...);
   API GetFileSize(void *, int *);
   API ReadFile(void *, void *, int, void *, int);
   API WriteFile(void *, void *, int, void *, void *);
   API CloseHandle(void *);
   API SetFilePointer(void *, int, int*, int);
   API SetConsoleTextAttribute(void*, word);


# ifdef UNDER_CE
   APP CreateFileW(wchar_t *, int, int, void *, int, int, void *);
   API DeleteFileW(void *);
# else
   APP CreateFileW(wchar_t *, int, int, void *, int, int, void *);
   API DeleteFileW(wchar_t *);
   APP CreateFileA(char *, int, int, void *, int, int, void *);
   API DeleteFileA(char *);
   API WriteConsoleA(void *, void *, int, void *, void *);
   API ExitProcess(int);
   APP GetStdHandle(int);
# endif
}
/*struct z_mutex
{
   bool busy;
   z_mutex() { busy = false; }
   void open()
   {
      while (busy) delay(0, 10);
      busy = true;
   }
   void close()
   {
      busy = false;
   }
};*/

//void * malloc(int i);
//void free(void *a);
//#define malloc(i) LocalAlloc(0, i);

//void * malloc(int i) { return LocalAlloc(0, i); }
//void free(void *a) { LocalFree(a); }
# ifdef UNDER_CE
wchar_t *wide_cvt = 0;
wchar_t *cvt_wide(char *s)
{
   if (wide_cvt == 0) wide_cvt = new wchar_t[255];
   wchar_t *R = wide_cvt;
   loop
   {
      *R++ = (*s & 0xff);
      if (*s++ == 0) break;
   }
   return wide_cvt;
}
# endif

void * CreateFileX(void* s, int a, int b, void *c, int d, int e, void *f)
{
# ifdef UNDER_CE
   wchar_t *C = cvt_wide((char*) s);
   return CreateFileW((wchar_t*) C, a, b, c, d, e, f); 
# else
   return CreateFileA((char*)s, a, b, c, d, e, f);
# endif
}

int DeleteFileX(void *n)
{
# ifdef UNDER_CE
   wchar_t *c = cvt_wide((char*) n);
   return DeleteFileW((wchar_t*)c);
# else
  return DeleteFileA((char*)n);
# endif
}

bool fileexists(wchar_t *n)
{
   bool B;
   void * F = CreateFileW(n, 1<<31, 1, 0, 3, 0, 0);
   B = ((int) F != -1);
   CloseHandle(F);
   return B;
}

bool fileexists(char *n)
{
   bool B;
   void * F = CreateFileX(n, 1<<31, 1, 0, 3, 0, 0);
   B = ((int) F != -1);
   CloseHandle(F);
   return B;
}

# ifdef _MSC_VER

//int main(int, char**);

#ifdef CONSOLE
void mainCRTStartup()
{
  main(0, 0);
}
#endif

extern "C"
{
   int _fltused() { return 0;};
//   int _ftol() { return 0;};
}

#ifndef UNDER_CE
extern "C"
{
   __declspec(naked) long _ftol(double x)
   {
        __asm
        {
            fistp    DWORD PTR [esp-12]
            mov eax, DWORD PTR [esp-12]
            mov ecx, DWORD PTR [esp-8]
            ret
        }
   }
   #ifndef __INTEL_COMPILER
   __declspec(naked) void _ftol2(double x)   {   }
   #endif

}
# endif // UNDER_CE
# endif // _MSC_VER

float _sqrt (float number)
{
   long i;
   float f = 1.5, x = number/2, y = number;
   i  = * ( unsigned long * ) &y;
   i   = 0x5f3759df - ( i >> 1 );
   y = * ( float * ) &i;
   y = y * (f - x*y*y);
   return number * y;
}

/*struct buf
{
   char *p;
   int size;
   
   buf()  { p = 0; }
  ~buf()  { delete(p); p = 0; }

   void operator () (int n)
   {
      if (p) free(p);
      p = (char*) malloc(n);
      size = n;
   }

   char & operator [] (int n)
   {
      return p[n];
   }

   char * operator * ()
   {
      return p;
   }

   int operator ! ()
   {
      return size;
   }
};*/

void load(char *n, void* data, int size, int from_pos = 0)
{
   void * F = CreateFileX(n, 1<<31, 1, 0, 3, 0, 0);
   int rd;
   rd = GetFileSize(F, 0);
   SetFilePointer(F, from_pos, 0, 0);
   ReadFile(F, data, size, &rd, 0);
   CloseHandle(F);
}

int filesize(char * n)
{
   void * F = CreateFileX(n, 1<<31, 1, 0, 3, 0, 0);
   int R = GetFileSize(F, 0);
   CloseHandle(F);
   return R;
}

void appendfile(char *n, void *data, int size)
{
   void * F = CreateFileX(n, 1<<30, 1, 0, 4, 0, 0);
   SetFilePointer(F, 0, 0, 2);
   int w;
   WriteFile(F, data, size, & w, 0);
   CloseHandle(F);
}

void deletefile(char *n)
{
   DeleteFileX(n);
}

void concolor(int c)
{
#ifndef UNDER_CE
   static void *C = 0;
   if (!C) C = GetStdHandle(-11);
   SetConsoleTextAttribute(C, c);
#endif
}

extern "C" void my_printf(char *c, ...)
{
 # ifndef UNDER_CE
   static void *C = 0;
   char *V = (char*)&c + 4;
   if (!C) C = GetStdHandle(-11);
   int E;
   while (*c)
   {
      if (*c == '%')
      {
         c++;
         if (!*c) break;
         static char *q = 0;
         if (!q) q = new char [20];
         for (int g = 0; g < 20; g++) q[g] = '.';
         char * t = q;
         if (*c == 'i')
           tostr(q, *(int*)V), V += 4;
         else if (*c == 'z')
           tostr(q, *(u64*)V), V += 8;
         else if (*c == 'u')
           tostr(q, *(unsigned int*)V), V += 4;
         else if (*c == 'f')
           tostr(q, *(double*)V), V += 8;
         else if (*c == 's')
            t = ((char**)V)[0], V += 4;
         else if (*c == 'c')
            q[0] = *(char*)V, q[1] = 0, V += 4;
         //WriteConsoleA(C, t, len(t), 0, 0);
         WriteFile(C, t, len(t), &E, 0);
      }
      //else WriteConsoleA(C, c, 1, 0, 0);
      else WriteFile(C, c, 1, &E, 0);
      c++;
   }
 # endif
}
void (*cur_printf)(char*, ...) = my_printf;
#define printf cur_printf

#ifndef _STDIO_H_
void exit (int i = 0)
{
 # ifndef UNDER_CE
   ExitProcess(i);
 # endif
  #ifdef __GNUC__
  loop;
  #endif
}
#endif

#else// WIN32
#include "unix_api.l"
#endif // WIN32
#endif // low_api_l


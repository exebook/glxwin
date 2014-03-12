#ifndef unixapi_l
#define unixapi_l

#ifndef SKIP_STD

#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0
#define NULL 0

#ifdef LOWUNIXAPI
typedef int FILE;
#endif // LOWUNIXAPI
extern "C" {
#ifdef __GNUC__
#define __cdecl
#endif
#ifdef LOWUNIXAPI
  int    __cdecl fseek(FILE *, long, int);
  long   __cdecl ftell(FILE *);
  FILE * __cdecl fopen(char * , char *);
  int    __cdecl fclose(FILE *);
  int    __cdecl fread(void *, int, int, FILE *);
  int    __cdecl fwrite(void *, int, int, FILE *);
#endif // LOWUNIXAPI
//  FILE * __cdecl _wfopen(char * , char *);
//  int    __cdecl sprintf(char*, char*, ...);
}

#endif // SKIP_STD

typedef unsigned char byte;
/*typedef long long u64;

  byte ticks_bits[3] = { 0x0F, 0x31, 0xC3};
  u64 ( * ticks ) () = (u64 (*) ()) & ticks_bits;
*/
#define my_round(D) ((D - (int)D) >= 0.5) ? D + 1 : D

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

//#ifndef low_api_l

void deletefile(char *n)
{
 using namespace unixstuff;
 FILE *f;
 f = fopen(n, "wb");
 fclose(f);
}

bool fileexists(char *name)
{
 using namespace unixstuff;
 FILE *f = fopen(name, "r+b");
 bool b = f != NULL;
 if (b) fclose(f);
 return b;
}

void appendfile(char *filename, void *data, int size)
{
 using namespace unixstuff;
   FILE *f;
   if (fileexists(filename))
   {
     f = fopen(filename, "r+b");
   }
   else
   {
     f = fopen(filename, "wb");
   }
   fseek(f, 0, SEEK_END);
   fwrite(data, 1, size, f);
   fclose(f);
}

int filesize(char *n)
{
 using namespace unixstuff;
 FILE *f;
 f = fopen(n, "rb");
 if (f)
 {
 fseek(f, 0, 2);
 int t;
 t = ftell(f);
 fseek(f, 0, 0);
 fclose(f);
 return t;
 } else return 0;
}

int load(char *filename, char *s, int size)
{
 using namespace unixstuff;
  FILE *f;
  f = fopen(filename, "r");
  if (f != 0)
  {
    int l = fread(s, 1, size, f);
    fclose(f);
    return l;
  }
  else
  {
     size = 0;
     *s = 0;
     return 0;
  }
}

//#endif // low_api
#endif // unixapi_l


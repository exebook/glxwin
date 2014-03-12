int cmp(char *a, char *b)
{
 if (!a) return a == b;
 if (!b) return 1;
 while (*a == *b && *a && *b) { a++; b++; };
 if (*a == *b && *a == 0) return 1;
 return 0;
}

int len(char *c)
{
   int R = 0;
   while (*c++) R++;
   return R;
}

int len(wchar_t *c)
{
  int l = 0;
  if (c == 0) return 0;
  while (*c++)
     l++;
  return l;
}

int toint(char* digit)
{
   int result = 0;
   if (digit == 0) return 0;
   while (*digit >= '0' && *digit <= '9')
   {
      result = (result * 10) + (*digit - '0');
      digit++;
   }
   if (*digit != 0) { return 0; }
   return result;
}

u64 toint64(char* digit)
{
   u64 result = 0;
   if (digit == 0) return 0;
   while (*digit >= '0' && *digit <= '9')
   {
      result = (result * 10) + (*digit - '0');
      digit++;
   }
   if (*digit != 0) { return 0; }
   return result;
}

double dbl(char *S, int c)
{
   if (c == 0) return 0;
   double T = 1, P = 1;
   bool dot = false;
   char N[12], X[12];
   int n = 0, x = 0;
   for (int p = 0; p < c; p++)
   {
      if (S[p] == '-') P = -P;
      else if (S[p] == '.') dot = true;
      else if (S[p] >= '0' && S[p] <= '9')
      {
         if (dot) X[x++] = S[p]; else N[n++] = S[p];
      }
   }
   N[n++] = 0; X[x++] = 0;
   for (int i = 1; i < x; i++) T /= 10;
   return (toint(N) + (T * toint(X))) * P;
}

void tostr(char *s, unsigned int i)
{
   char *c = s;
   while (true)
   {
      unsigned int i10 = i / 10;
      *s = (i - (i10 * 10)) + '0';
      i = i10;
      s++;
      if (i == 0) { *s-- = 0; break; }
   }
   char *b = s;
   while (b > c)
   {
      char tmp = *b;
      *b-- = *c;
      *c++ = tmp;
   }
}

void tostr(char *s, u64 i)
{
   char *c = s;
   while (true)
   {
      u64 i10 = i / 10;
      *s = (i - (i10 * 10)) + '0';
      i = i10;
      s++;
      if (i == 0) { *s-- = 0; break; }
   }
   char *b = s;
   while (b > c)
   {
      char tmp = *b;
      *b-- = *c;
      *c++ = tmp;
   }
}

void tostr(char *s, int i)
{
   if (i < 0)
   {
      *s++ = '-';
      tostr(s, (unsigned int)-i);
   }
   else
     tostr(s, (unsigned int)i);
}

void tostr(char *s, double d)
{
   if (d < 0)
   {
      *s++ = '-';
      if ((int) (d * 100) != 0)
      tostr(s, -d);
      else *s++ = '0', *s++ = 0;
   }
   else
   {
      char z = ' ';
      tostr(s, (int)d);
      s += len(s);
      *s++ = '.';
      int N = 2;
      unsigned int Q = 1;
      while (N-- > 0) Q *= 10;
      double D = (d - (int)d) * Q;
      unsigned int n =  ((D - (int)D) >= 0.5) ? (unsigned)D + 1 : (unsigned)D;
      if (n == Q) n--;
      if (n < 10) *s++ = '0';
      tostr(s, n);
      if (z != ' ')
      {
         while (*s != 0) s++;
         *s++ = z;
         *s++ = 0;
      }
   }
}

void tostr2(char *s, double d)
{
   if (d < 0)
   {
      *s++ = '-';
      if ((int) (d * 100) != 0)
      tostr(s, -d);
      else *s++ = '0', *s++ = 0;
   }
   else
   {
      char z = ' ';
      //if (d > 1000) d /= 1000, z = ' ';
      if (d > 1000000)
      {
         d /= 1000000, z = 'M';
         if (d > 1000) d /= 1000, z = 'G';
         if (d > 1000) d /= 1000, z = 'T';
         if (d > 1000) d /= 1000, z = 'P';
         if (d > 1000) d /= 1000, z = 'E';
         if (d > 1000) d /= 1000, z = 'Z';
         if (d > 1000) d /= 1000, z = 'Y';
         if (d > 1000) d /= 1000, z = 'Q';
      }   
      tostr(s, (int)d);
      s += len(s);
      *s++ = '.';
      int N = 2;
      unsigned int Q = 1;
      while (N-- > 0) Q *= 10;
      double D = (d - (int)d) * Q;
      unsigned int n = ((D - (int)D) >= 0.5) ? (unsigned)D + 1 : (unsigned)D;
      if (n == Q) n--;
      if (n < 10) *s++ = '0';
      tostr(s, n);
      if (z != ' ')
      {
         while (*s != 0) s++;
         *s++ = z;
         *s++ = 0;
      }
   }
}

int length(char *s){ return len(s);};

#ifndef NO_LOWERCASE
template <typename T> void lowercase(T *p)
{
  while (*p)
  {
     if (*p >= 'A' && *p <= 'Z') *p += ('a' - 'A');
     p++;
  }
}
#endif

void uppercase(char *p)
{
  while (*p)
  {
    if (*p >= 'a' && *p <= 'z') *p -= ('a' - 'A');
    p++;
  }
}

void lowerrus(char *P)
{
  unsigned char *p = (unsigned char*)P; 
  while (*p)
  {
     if (*p >= 'A' && *p <= 'Z') *p += ('a' - 'A');
     if (*p >= 192 && *p <= 223) *p += (224 - 192);
     p++;
  }
}




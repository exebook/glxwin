/*str readln()
{
  str r;
  loop
  {
     int c = getchar();
     if (c == 13) break;
     if (c == 10) break;
     r / (char)c;
  }

  return r;
} */

str bhex(byte b)
{
    str s; char c;
    byte B;
    B = (b >> 4) & 0xF;
    if (B <= 9) c = B + '0'; else c = B - 10 + 'A'; s / c;
    B = b & 0xF;
    if (B <= 9) c = B + '0'; else c = B - 10 + 'A'; s / c;
    return s;
}

str hex_to_str(u32 c)
{
   str s;
   byte *b = (byte*)&c;
   s.ins(bhex(*b++), 0);
   s.ins(bhex(*b++), 0);
   s.ins(bhex(*b++), 0);
   s.ins(bhex(*b++), 0);
   return s;
}

void quick_hex(byte b, char *&dest)
{
    char c;
    byte B;
    B = (b >> 4) & 0xF;
    if (B <= 9) c = B + '0'; else c = B - 10 + 'A'; *dest++ = c;
    B = b & 0xF;
    if (B <= 9) c = B + '0'; else c = B - 10 + 'A'; *dest++ = c;
}

str hex_dump(str src)
{
   str d;
   d(!src * 2);
   char *w = *d, *a = *src, *e = a + !src;
   while (a < e)
     quick_hex(*a++, w);
   return d;
}

str hex_undump(str src)
{
// Herzliebster Jesu, was hast du verbrochen
   str d;
   static str hash;
   if (!hash == 0)
   {
      hash(256);
      hash.fill(100);
      for (int a = '0'; a <= '9'; a++) hash[a] = a - '0';
      for (int b = 'a'; b <= 'f'; b++) hash[b] = b - 'a' + 10;
      for (int c = 'A'; c <= 'F'; c++) hash[c] = c - 'A' + 10;
   }
   d(!src / 2);
   char *w = *d, *a = *src, *e = a + !src;
   char* p = hash.p->p;
   while (a < e)
   {
      byte A, B;
      loop { A = p[*a++]; if (A != 100 || a >= e)  break; }
      loop { B = p[*a++]; if (B != 100 || a >= e)  break; }
      if (a > e) break;
     // A = p[*a++];
     // B = p[*a++];
      *w++ = (A << 4) + B;
   }
   d(w - *d);
   return d;
}

u32 str_to_hex(str s)
{
  u32 R = 0;
  for (int i = 0; i < !s; i++)
  {
     R <<= 4;
     byte c = 0;
     if (s[i] >= '0' && s[i] <= '9') c = s[i] - '0';
     else if (s[i] >= 'a' && s[i] <= 'f') c = s[i] - 'a' + 10;
     else if (s[i] >= 'A' && s[i] <= 'F') c = s[i] - 'A' + 10;
     R |= c;
  }
  return R;
}

byte hexhalf(char *s)
{
   byte c = 0;
   if (*s >= '0' && *s <= '9') c = *s - '0';
   else if (*s >= 'a' && *s <= 'f') c = *s - 'a' + 10;
   else if (*s >= 'A' && *s <= 'F') c = *s - 'A' + 10;
   return c;
}

byte hex1(char *s)
{
  byte R = 0;
  byte c;
  c = hexhalf(s);
  R |= c;
  R <<= 4;
  s++;
  c = hexhalf(s);
  R |= c;
  return R;
}

word hex2(char *s)
{                         
   return (hex1(& s[0]) << 4) | hex1(& s[1]);
}

int backpos(str s, char sub)
{
   for (int i = !s - 1; i >= 0; i--)
   {
     if (s[i] == sub) return i;
   }
   return -1;
}

str fileext(str s)
{
   int i = backpos(s, '.');
   if (i > 0)
   {
      s.del(0, i + 1);
   }
   else s = "";
   for (i = 0; i < !s; i++)
     if (s[i] >= 'A' && s[i] <= 'Z') s[i] += 'a' - 'A';
   return s;
}

int ord(char c)
{
   return c & 0xff;
}

template <typename T>
T bite_t(T &s, T separator)
{
   T R;
   int i = s.pos(separator);
   if (i < 0)
   {
      R = s;
      s(0);
   }
   else
   {
      R(i);
      if (!R > 0)
      {
         move(*s, *R, !R * R.charsize());
      }
      s.del(0, i + !separator);
   }
   return R;
}

str bite(str &s, str separator)
{
   return bite_t(s, separator);
}

#ifndef NO_WSTR
wstr bite(wstr &s, wstr separator)
{
   return bite_t(s, separator);
}

wstr wbite(wstr &s, wstr separator)
{
   return bite_t(s, separator);
}

#endif // NO_WSTR

int earlier(str s, str a, str b)
{
   int A = s.pos(a), B = s.pos(b);
   if (A < 0)
   {
      if (B < 0) return -1;
      return 1;
   }
   if (B < 0 || A < B) return 0;
   return 1;
}

str tostr(int i4)
{
   str s = i4;
   return s;
}

double dbl(str s)
{
   if (!s == 0) return 0;
   double T = 1, P = 1;
   bool dot = false;
   str N, X; int n = 0, x = 0; N(!s); X(!s);
   each (p, s)
   {
      if (s[p] == '-') P = -P;
      else if (s[p] == '.') dot = true;
      else if (s[p] >= '0' && s[p] <= '9')
      {
         if (dot) X[x++] = s[p]; else N[n++] = s[p];
      }
   }
   N(n); X(x);
   each (i, X) T /= 10;
   //
   n = ~N;
   x = ~X;
   return (n + (T * x)) * P;
}

int toint(str digit)
{
   return toint(*digit);
}

template <typename T>
void invert(T &s)
{
  int a = 0, b = !s - 1, e = !s;
  s(!s + 1);
  while (a <= b)
  {
     s[e] = s[a];
     s[a] = s[b];
     s[b] = s[e];
     a++; b--;
  }
  s(!s - 1);
}



# ifndef NO_WSTR
# ifndef utf_cpp
# define utf_cpp

word UTF = 0xFEFF;

int utf_offsets[6] = { 0x0UL, 0x3080UL, 0xE2080UL, 0x3C82080UL, 0xFA082080UL, 0x82082080UL };

byte utf_trail(int i)
{
   if (i < 192) return 0;
   if (i < 224) return 1;
   if (i < 240) return 2;
   if (i < 248) return 3;
   if (i < 252) return 4;
   return 5;
}

int UTF2W (byte* src, byte* SB, word* dst, word* TB)
{
    byte* S = src;
    word* T = dst;
    while (S < SB)
    {
	int ch = 0;
	word X = utf_trail(*S);
	if (S + X >= SB) break;
        int x = X;
        while (x-- > 0) ch += *S++, ch <<= 6;
        ch += *S++;
	ch -= utf_offsets[X];
	if (T >= TB) { S -= (X+1); break; }
	if (ch <= 0xFFFF)
        {
	    if (ch >= 0xD800 && ch <= 0xDFFF) *T++ = 0xFFFD;
            else *T++ = (word)ch;
	}
        else if (ch > 0x0010FFFF) *T++ = 0xFFFD;
	else
        {
	    if (T + 1 >= TB) { S -= (X+1); break; }
	    ch -= 0x0010000UL;
	    *T++ = (word)((ch >> 10) + 0xD800);
	    *T++ = (word)((ch & 0x3FFUL) + 0xDC00);
	}
    }
    return T - dst;
}

wstr utf2w(str s)
{
   if (!s >= 3 && (byte)s[0] == 0xef && (byte)s[1] == 0xbb && (byte)s[2] == 0xbf)
     s.del(0, 3);
   wstr R;
   R(!s);
   byte * ab = (byte*) *s;
   ab += !s;
   word * bb = (word*) *R; bb += !R;
   int L = UTF2W((byte*) *s, ab, (word*) *R, bb);
   R(L);
   return R;
}

int wutf_size(wstr S, bool prefix = false)
{
   int n = 0;
   int c;
   each (i, S)
   {
      c = S[i];
      if (c > 0x7F) { if (c > 0x7FF) n++; n++; }
      n++;
   }
   if (prefix) n += 2;
   return n;
}

int wutf(char *D, int DD, wchar_t *S, int SS)
{
  int i = 0, n = 0, c;
  if (S == 0) return 0;
  if (D != 0)
  {
    while ((i < SS) && (n < DD))
    {
      c = S[i++];
      if (c <= 0x7F) D[n++] = c;
      else if (c > 0x7FF)
      {
        if (n + 3 > DD) break;
        D[n] = 0xE0 | (c >> 12);
        D[n + 1] = 0x80 | ((c >> 6) & 0x3F);
        D[n + 2] = 0x80 | (c & 0x3F);
        n += 3;
      }
      else
      {
        if (n + 2 > DD) break;
        D[n] = 0xC0 | (c >> 6);
        D[n + 1] = 0x80 | (c & 0x3F);
        n += 2;
      }
    }
    //if (n > DD) n = DD - 1;
    D[n] = 0;
  }
  else
  {
    while (i < SS)
    {
      c = S[i++];
      if (c > 0x7F) { if (c > 0x7FF) n++; n++; }
      n++;
    }
  }
  return n;
}

str w2utf(wstr w, bool prefix = false)
{
  str R;
  if (!w == 0) return R;
  R(wutf_size(w, prefix));
  char * e = *R;
  if (prefix) e += 2;
  wutf(e, !R, *w, !w);
  if (prefix) move(& UTF, *R, 2);
  return R;
}

# endif // utf_cpp
#endif // NO_WSTR


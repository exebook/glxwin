struct wc
{
  word code;
  word c;
wc()
{
  c = 0;
}

wc(word w)
{
  c = 0; code = w;
}

wchar_t * operator * ()
{
  return (wchar_t *) this;
}

};

const char *str_no_null = "\0\0\0\0";

char up_char(char c)
{
   if (c >= 'a' && c <= 'z')
     c -= 'a' - 'A';
   return c;
}

wchar_t up_char(wchar_t c)
{
   if (c >= 'a' && c <= 'z')
     c -= 'a' - 'A';
   return c;
}

wchar_t to_unicode(char c)
{
   int q = c & 0xff;
   word w = 0;
   w += c;
   char A = 0xC0;
   char a = 0xE0;
   char YA = 0xDF; 
   char ya = 0xFF; 
   if (c >= A && c <= YA)  w += 0x0410 - A;
   if (c >= a && c <= ya) w += 0x0410 - A;
   if (q == 184)  w = 0x0451;
   else if (q == 0x97) w = 0x2014;
   else if (q == 0xAB) w = 0xAB;
   else if (q == 0xBB) w = 0xBB;
   else if (q == 0x91) w = 0x5C;//'\'';
   else if (q == 0x92) w = 0x5C;//'\'';
   else if (q == 0x84) w = '"';
   else if (q == 0x93) w = '"';
   else if (q == 0x85) w = 0x2026; // three dots
   return w;
}

char to_ascii(wchar_t w)
{
   char c;
   char a = 0xE0; // rusA
   if (w >= 0x0410)
   c = (char) w - (0x0410 - a);
   else if (w == 0x0451) c = 184;
   else if (w == 0x2014) c = 0x97;
   else c = w & 0xff;
   return c;
}



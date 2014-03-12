#ifndef join_cpp
#define join_cpp

void assign(strings &L, str V, str D)
{
    str s, q;
    int start, stop, i;
    L(V.strcount(D) + 1);
    if (!V == 0) L(0);
    i = 0;
    start = 0;
    while (i < !L)
    {
      stop = V.find(start, *D, !D);
      if (stop < 0) stop = !V;
      s = "";
      s(stop - start);
      if (!s > 0) move(&V[start], *s, !s * s.charsize());
      L[i++] = s;
      start = stop + !D;
    }
}

str join(strings &L, str D)
{
   str r;
   if (!L == 0) return r;
   int m = 0, u, W = L[0].charsize();
   each(i, L)
     m += !L[i];
   m += (!L - 1) * !D;
   r.resize(m);
   u = 0;
   each(j, L)
   {
      if (!L[j] > 0) move(&L[j][0], &r[u], !L[j] * W);
      u += !L[j];
      if (j < !L - 1)
      {
         if (!D > 0) move(&D[0], &r[u], !D* W);
         u += !D * W;
      }
   }
   return r;
}

void assign_text(strings &l, str s)
{
   str crlf;
   int a = -1;
   each (i, s)
   {
       if (s[i] == '\r' || s[i] == '\n') { crlf / s[i]; }
       if (a >= 0) break;
       if (!crlf == 2) break;
       if (!crlf > 0) a = i;
   }
   if (!crlf > 0)
     assign(l, s, crlf); else { l(0); l << s; }
}

//#ifdef wstr_cpp

#ifndef NO_WSTR
void assign(wstrings &L, wstr V, wstr D)
{
    wstr s, q;
    int start, stop, i;
    L(V.strcount(D) + 1);
    if (!V == 0) L(0);
    i = 0;
    start = 0;
    while (i < !L)
    {
      stop = V.find(start, *D, !D);
      if (stop < 0) stop = !V;
      s = "";
      s(stop - start);
      if (!s > 0) move(&V[start], &s[0], !s * s.charsize());
      L[i++] = s;
      start = stop + !D;
    }
}

wstr join(wstrings &L, wstr D)
{
   wstr r;
   if (!L == 0) return r;
   int m = 0, u, W = L[0].charsize();
   each(i, L) m += !L[i];
   m += (!L - 1) * !D;
   r.resize(m);
   u = 0;
   //for (i = 0; i < !L; i++)
//   int m1 = 0;
   each(j, L)
   {
//      m1 += L.p[j]->p->size * W;
//      printf("%i, %i, %i, %i, %i\n",
//        m*2, m1, & L.p[j]->p[0], & r.p->p[u], L.p[j]->p->size * W);
      int x = !L[j] * W;
      if (x) move(& L[j][0], & r[u], x);
      u += !L[j];
      if (j < !L - 1)
      {
         move(&D[0], &r[u], !D * W);
         u += !D;
      }
   }
   return r;
}

void assign_text(wstrings &L, wstr w)
{
    L(0);
    if (!w == 0) return;
    wchar_t *e = &w[-1], *a = *w, *b = a;
    loop
    {
       if (a > e)
       {
           wstr t;
           t(a - b);
           move(b, *t, !t * 2);
           L << t;
           break;
       }
       if (*a == 10 || *a == 13)
       {
          wstr t;
          t(a - b);
          move(b, *t, !t * 2);
          L << t;
          //if (a == e) break;
          if (a < e && (a[0] == 13 && a[1] == 10)) a++;
          a++;
          b = a;
          continue;
       }
       a++;
    }
}
void assign_text_old(wstrings &l, wstr w)
{
   wstr crlf;
   int a = -1;
   each (i, w)
   {
       if (w[i] == L'\n' && !crlf == 1 && crlf[0] == L'\n') break;
       if (w[i] == L'\r' || w[i] == L'\n')
       {
          crlf / w[i];
       }
       if (a >= 0) break;
       if (!crlf == 2) break;
       if (!crlf > 0) a = i; 
   }
   if (!crlf > 0)
     assign(l, w, crlf); else { l(0); l << w; }  
}

#endif // NO_wstr

void destring(str &s, char str_char, bool direction)
{
   static strings L;
   if (direction)
   {
      L(0);
      int a = 0;
      str t;
      bool q = false;
      each (i, s)
      {
         if (s[i] == str_char)
         {
            q = !q;
            if (q)
            {
                t / s.sub(a, i - a) / (char) 1;
                a = i;
            }
            else
            {
                L << s.sub(a, i - a + 1);
                a = i + 1;
            }
            continue;
         }
      }
      t / s.sub(a, !s - a);
      s = t;
   }
   else
   {
      each(l, L)
      s.replace((char)1, L[l], 0, 0, 1); 
   }
}


#endif // join_cpp

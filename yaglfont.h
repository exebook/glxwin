int zzz = 0;

namespace yaglfont {
	double FIX = 1.0;
	FT_Library  library = 0;
	struct char_glass { char_glass() : loaded(false) {};
		bool loaded; double full, left, ascent, id, w, h; double x1, x2, y1, y2; };

	byte getpix(byte *A, int w, int x, int y) {
		return A[x + y * w];
	}

	int how_many_fit(int wh, int w, int h) {
		int line = wh / w;
		int lines = wh / h;
		return line * lines;
	}

	int calc_square(int count, int w, int h) {
		int Q = 1;
		for (int i = 0; i < 32; i++) {
			int fit = how_many_fit(Q, w, h);
			if (fit >= count) return Q;
			Q *= 2;
		}
		return -1;
	}

	struct FontTex {
		int wh, line_h, total_glyphs, added_glyphs;
		int add_at_x, add_at_y;
		byte* PIX;

		void init(int count, int w, int h) {
			total_glyphs = count; added_glyphs = 0;
			line_h = h+4;
			wh = calc_square(count, w+1, h+1);
			PIX = new byte[wh * wh];
			byte Q = 0;
			for (int i = 0; i < wh * wh; i++) {
				PIX[i] = Q;
				if (Q==0) Q = 255; else Q = 0;
			}
			add_at_x = 0; add_at_y = 0;
		}

		void setpix(int x, int y, byte value) {
			PIX[x + y * wh] = value;
		}

		void add(char_glass *G, byte *A) {
			if ((add_at_x + G->w) >= wh-10) { // can -10 be zero or -1?
				add_at_x = 0, add_at_y += line_h;
				if (add_at_y + line_h > wh)
					printf("font does not fit into the texture\n"), print(), exit(1);
			}
			G->y1 =  add_at_y / (double)wh;
			G->y2 =  (add_at_y + G->h) / (double)wh;
			G->x1 =  add_at_x / (double)wh;
			G->x2 =  (add_at_x + G->w) / (double)wh;
			add_at_x += G->w ;
			added_glyphs++;
		}

		void print() {
			printf("2D{%i of %i}, size: [%i/%i] position: %i:%i = %f %% space\n", 
				added_glyphs, total_glyphs, wh, line_h, add_at_y, add_at_x,
			(double)(add_at_y+line_h) / (wh / 100) );
		}
	};

	struct Font {
		FT_Face face;
		arr <char_glass*> db;
		struct Metric { str name; double size, a, d, h; } metric;
		FontTex fonttex;
		GLuint T;
		double extra_inter_char, tune_y;

		void create(str Name, double Size) {
			extra_inter_char = 0;
			if (Name.pos("fixed7") >= 0) extra_inter_char = 1;
			tune_y = 0;
			metric.name = Name, metric.size = Size;
			if (library == 0) FT_Init_FreeType(&library);
			if (!db == 0) {
				FT_New_Face(library, *Name, 0, &face);
				if (face == 0) printf("Font '%s' not found.\n", *Name);
				FT_Set_Pixel_Sizes(face, Size * FIX, 0);
				db(0x10000);
				each(i, db) db[i] = 0;
				FT_Size_Metrics *m = & face->size->metrics;
				metric.a = m->ascender / 64;
				metric.d =-m->descender / 64;
				metric.h = m->height / 64;
				FT_ULong  i;
				FT_UInt   gindex;
				int COUNT = 0;
				i = FT_Get_First_Char( face, &gindex );
				while ( gindex != 0 ) COUNT++, db[i] = new char_glass, i = FT_Get_Next_Char( face, i, &gindex );
				fonttex.init((double)COUNT, m->x_ppem, m->y_ppem);
				glGenTextures( 1, &T );
				glBindTexture( GL_TEXTURE_2D, T );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, fonttex.wh, fonttex.wh, 0, GL_ALPHA, GL_UNSIGNED_BYTE,  fonttex.PIX);
				delete [] fonttex.PIX;
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glBindTexture( GL_TEXTURE_2D, -1 );
			}
		}

		unsigned char* load_char(wchar_t ch) {
			char_glass *G = db[ch];
			FT_GlyphSlot slot;
			int e;
			e = FT_Load_Char(face, ch, FT_LOAD_TARGET_LIGHT
			// | FT_LOAD_FORCE_AUTOHINT
			);//FT_LOAD_DEFAULT );
			e = FT_Render_Glyph(face->glyph, 
				FT_RENDER_MODE_NORMAL
			);
			slot = face->glyph;
			G->w = slot->bitmap.width, G->h = slot->bitmap.rows;
			G->full = slot->metrics.horiAdvance / 64;
			G->left = slot->metrics.horiBearingX / 64;
			G->ascent = slot->metrics.horiBearingY / 64;
			G->loaded = true;
			return slot->bitmap.buffer;
		}
	};

	void zoom(byte *A, byte *B, int w, int h, int W, int H) {
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++)
				B[x + y * W] = A[ x + y * w ];
	}

	void prepare_char(Font &F, char_glass *G, int ch) {
		if (G->loaded == false) {
			byte *PIX = F.load_char(ch);
			G = F.db[ch];
			F.fonttex.add(G, PIX);
			glBindTexture( GL_TEXTURE_2D, F.T );
			int P = 4; while (P < G->w) P <<= 1; // artifacts if P=1,2
			while (P < G->h) P <<= 1;
			int Q = P;
			//int Q = 4; while (Q < G->h) Q <<= 1;
			byte *ZOOM = new byte[P*Q];
//			for (int i = 0; i < P*Q; i ++) ZOOM[i] = 0xff;
			zoom(PIX, ZOOM, G->w, G->h, P, Q);
			glTexSubImage2D( GL_TEXTURE_2D, 0, 
				F.fonttex.add_at_x - G->w, 
				F.fonttex.add_at_y, 
				P, Q, GL_ALPHA, GL_UNSIGNED_BYTE, ZOOM);
			delete[] ZOOM;
		}
	}

	int draw_char(Font &F, int ch, int x) {
		char_glass *G = F.db[ch];
		if (G == 0) {
//			printf(".%i\n", zzz++);
			return F.db[32]->full + F.extra_inter_char;
		}
		F.db[32];//
		if (ch < 32) ch = 32;
		int R = 3;
		if (ch > 32) {
			if (G->loaded == false) prepare_char(F, G, ch);
			double w1 = G->w; int h1 = G->h;
			double y = -G->ascent + F.metric.a;
			y += F.tune_y;
			// TODO: know more about metrics and sizes do some tests with many fonts
			x += G->left + F.extra_inter_char;
			glTexCoord2f(G->x1, G->y1); glVertex2f(0+x, 0+y);
			glTexCoord2f(G->x2, G->y1); glVertex2f(w1+x, 0+y);
			glTexCoord2f(G->x2, G->y2); glVertex2f(w1+x, h1+y);

			glTexCoord2f(G->x2, G->y2); glVertex2f(w1+x, h1+y);
			glTexCoord2f(G->x1, G->y2); glVertex2f(0+x, h1+y);
			glTexCoord2f(G->x1, G->y1); glVertex2f(0+x, 0+y);
			R = G->full + F.extra_inter_char;
		} else R = F.db[32]->full + F.extra_inter_char;
		return R;
	}

	int* size_str(Font &F, wstr s) {
		static int wh[2];
		wh[0] = 0; wh[1] = F.metric.h;
		each (i, s) {
			int ch = s[i];
			char_glass *G = F.db[ch];
			if (G) {
				if (G->loaded == false) prepare_char(F, G, ch);
				wh[0] += G->full + F.extra_inter_char;
			}
		}
		return wh;
	}

	void draw_str(Font &F, wstr s) {
		size_str(F, s);
		int x = 0;
		glBegin(GL_TRIANGLES);
		each (i, s) {
			x += draw_char(F, s[i], x);
		}
		glEnd();
	}

	void draw_text(Font &F, wstr s, int *back) {
		size_str(F, s);
		int x = 0;
		glBegin(GL_TRIANGLES);
		each (i, s) {
			glColor4ubv((GLubyte*)back++);
			x += draw_char(F, s[i], x);
		}
		glEnd();
	}

	void draw_line(Font &F, wstr s, unsigned int *colors) {
		// color converted: 0x10001fff -> 0xee00000+ 0xeeffffff
		glDisable( GL_TEXTURE_2D );
		size_str(F, s);
		glBegin(GL_TRIANGLES);
		unsigned int c, cf, cb;
		int x = 0, h1 = F.metric.h;
		each (i, s) {
			c = colors[i];
			cb = (c & 0xf0000) >> 16 | (c & 0xff0000) >> 12
			| (c & 0xff00000) >> 8 | (c & 0xf000000) >> 4
			| (~c & 0xf0000000) >> 0 | (~c & 0xf0000000) >> 4;
			glColor4ubv((GLubyte*) & cb);
			int ch = s[i]; if (ch < 32 || F.db[ch] == 0) ch = 32;
			char_glass *G = F.db[ch];
			if (G == 0) G = F.db[32];
			{
				int w1 = F.db[ch]->full + F.extra_inter_char;
				int y = 0;
				glVertex2i(0+x, 0+y); glVertex2i(w1+x, 0+y); glVertex2i(w1+x, h1+y);
				glVertex2i(w1+x, h1+y); glVertex2i(0+x, h1+y); glVertex2i(0+x, 0+y);
				x += w1;
			}
		}
		glEnd();
		x = 0;
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, F.T );
		glBegin(GL_TRIANGLES);
		each (i, s) {
			c = colors[i];
			cf = (c & 0xff) << 4 | c & 0xf | (c & 0xff0) << 8
			| (c & 0xf00) << 12 | (~c & 0xf000) << 16 | (~c & 0xf000) << 12;
			glColor4ubv((GLubyte*) & cf);
			int X = draw_char(F, s[i], x);
			x += X;
		}
		glEnd();
		glBindTexture( GL_TEXTURE_2D, -1 );
	}
}



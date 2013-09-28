#include "romaji.h"

using namespace std;

namespace {

struct utf8_kana_to_romaji_converter {

	enum consonant {

		// first block, part 1
		CONS_K,
		CONS_S,
		CONS_T,

		// first block, part 2
		CONS_G,
		CONS_Z,
		CONS_D,

		CONS_N,

		CONS_H,
		CONS_B,
		CONS_P,

		CONS_M,
		CONS_R,
		CONS_W,
		CONS_V,

		CONS_SH,
		CONS_CH,
		CONS_TS,
		CONS_F,
		CONS_J,

		CONS_Y,

		// non-printing
		SMALL_VOWEL,
		CONS_NONE

	};

	inline static const char *cons_to_romaji(core::unicode_char c) {
		static const char *CONS_ROMAJI[] = {

			"k",
			"s",
			"t",

			"g",
			"z",
			"d",

			"n",

			"h",
			"b",
			"p",

			"m",
			"r",
			"w",
			"v",

			"sh",
			"ch",
			"ts",
			"f",
			"j",

			"y"

		};
		return CONS_ROMAJI[c];
	}

	enum vowel {
		VOWEL_A,
		VOWEL_I,
		VOWEL_U,
		VOWEL_E,
		VOWEL_O
	};

	inline static char vowel_to_romaji(core::unicode_char v) {
		static char VOWEL_ROMAJI[] = {'a', 'i', 'u', 'e', 'o'};
		return VOWEL_ROMAJI[v];
	}

	enum {
		HIRAGANA_START  = 0x3041,
		HIRAGANA_END    = 0x3096,
		EXTRA_HIRAGANA_END = 0x309a,
		KATAKANA_START  = 0x30a1,
		KATAKANA_END    = 0x30fa,
		SMALL_TSU       = 0x3063,
		FULLWIDTH_BEGIN = 0xff01,
		FULLWIDTH_END   = 0xff5e,
		PRINTABLE_BEGIN = '!',
		COMMA           = 0x3001,
		PERIOD          = 0x3002,
		MIDDLE_DOT      = 0x30fb,
		VOWEL_BLOCK_START = 0x3041,
		VOWEL_BLOCK_END = 0x304a,
		K_BLOCK_START   = 0x304b,
		T_BLOCK_END     = 0x3069,
		N_BLOCK_START   = 0x306a,
		N_BLOCK_END     = 0x306e,
		H_BLOCK_START   = 0x306f,
		H_BLOCK_END     = 0x307d,
		M_BLOCK_START   = 0x307e,
		M_BLOCK_END     = 0x3082,
		Y_BLOCK_START   = 0x3083,
		Y_BLOCK_END     = 0x3088,
		R_BLOCK_START   = 0x3089,
		R_BLOCK_END     = 0x308d,
		W_BLOCK_START   = 0x308f,
		W_BLOCK_END     = 0x3092,
		SMALL_WA        = 0x308e,
		V_BLOCK_START   = 0x3097,
		V_BLOCK_END     = 0x309a,
		KANA_VU         = 0x3094,
		KANA_N          = 0x3093,
		LENGTH_MARKER   = 0x30fc
	};

	inline static char small_tsu_romaji(core::unicode_char c) {
		if(c == CONS_CH) return 't';
		else return cons_to_romaji(c)[0];
	}

	inline static bool uses_glide(core::unicode_char c) {
		return c != CONS_SH && c != CONS_CH && c != CONS_J;
	}

	inline static bool uses_w_glide(core::unicode_char c) {
		return c == CONS_K || c == CONS_G;
	}

	inline void pop_vowel() {
		if(!romaji.empty()) romaji.erase(romaji.end() - 1);
	}

	inline char last_char() const {
		return romaji[romaji.size() - 1];
	}

	inline static char vowel_to_cons(core::unicode_char v) {
		switch(v) {
		case VOWEL_I: return CONS_Y;
		case VOWEL_U: return CONS_W;
		default: return v;
		}
	}

	utf8_kana_to_romaji_converter(const char *kana, std::string &romaji)
		: kana(kana)
		, romaji(romaji)
	{}

	inline bool has_next_char() const {
		return *kana;
	}

	inline void get_char() {
		kana = core::read_utf8_char(kana, curr);
	}

	void convert() {
		static const core::unicode_char none = 100;
		bool small_tsu = false, was_small_tsu;
		core::unicode_char cons = none, vowel = none, prev_cons = none, prev_vowel = none;
		while(has_next_char()) {
			get_char();

			was_small_tsu = small_tsu;
			small_tsu = false;

			prev_cons = cons;
			prev_vowel = vowel;

			/* Just map the full-width block to its ASCII equivalent. */
			if(FULLWIDTH_BEGIN <= curr && curr <= FULLWIDTH_END) {
				romaji += (char) (curr + PRINTABLE_BEGIN - FULLWIDTH_BEGIN);
			}
			/* Handle some punctuation characters specially. */
			else if(curr == PERIOD) {
				romaji += '.';
			}
			else if(curr == COMMA) {
				romaji += ',';
			}
			else if(curr == MIDDLE_DOT) {
				romaji += ' ';
			}
			/* Handle the long vowel marker. */
			else if(curr == LENGTH_MARKER) {
				if(!romaji.empty()) romaji += last_char();
			}
			else {

				/* Map everything in the katakana block to the
				hiragana block for simplicity. */
				bool is_katakana = KATAKANA_START <= curr && curr <= KATAKANA_END;
				if(is_katakana) {
					curr += HIRAGANA_START - KATAKANA_START;
				}

				/* Check that the character is in range, else use blank. */
				if(!(
					HIRAGANA_START <= curr &&
					(curr <= HIRAGANA_END || (is_katakana && curr <= EXTRA_HIRAGANA_END))
				))
				{
					continue;
				}

				/* Handle small tsu specially. */
				if(curr == SMALL_TSU) {
					small_tsu = true;
					continue;
				}

				/* Handle n */
				if(curr == KANA_N) {
					romaji += 'n';
					continue;
				}

				bool is_small_cons = false;

				/* Do some trickery with the Unicode code points to get the
				consonant and vowel sound of the current character. */
				if(curr <= VOWEL_BLOCK_END) {
					curr -= VOWEL_BLOCK_START;
					cons = SMALL_VOWEL + curr % 2;
					vowel = curr / 2;
				}
				else if(curr <= T_BLOCK_END) {
					curr -= curr > SMALL_TSU;
					curr -= K_BLOCK_START;
					cons = CONS_K + curr / 10 + (CONS_G - CONS_K) * (curr % 2);
					vowel = (curr / 2) % 5;
				}
				else if(curr <= N_BLOCK_END) {
					cons = CONS_N;
					vowel = curr - N_BLOCK_START;
				}
				else if(curr <= H_BLOCK_END) {
					curr -= H_BLOCK_START;
					cons = CONS_H + curr % 3;
					vowel = curr / 3;
				}
				else if(curr <= M_BLOCK_END) {
					cons = CONS_M;
					vowel = curr - M_BLOCK_START;
				}
				else if(curr <= Y_BLOCK_END) {
					curr -= Y_BLOCK_START;
					cons = CONS_Y;
					vowel = curr / 2;
					vowel += (vowel > 0) + (vowel > 1);
					is_small_cons = (curr % 2 == 0);
				}
				else if(curr <= R_BLOCK_END) {
					cons = CONS_R;
					vowel = curr - R_BLOCK_START;
				}
				else if(curr == SMALL_WA) {
					cons = CONS_W;
					vowel = VOWEL_A;
					is_small_cons = true;
				}
				else if(curr <= W_BLOCK_END) {
					cons = CONS_W;
					vowel = curr - W_BLOCK_START;
					vowel += vowel >= 2;
				}
				else if(curr == KANA_VU) {
					cons = CONS_V;
					vowel = VOWEL_U;
				}
				else if(is_katakana && (V_BLOCK_START <= curr && curr <= V_BLOCK_END)) {
					cons = CONS_V;
					vowel = curr - V_BLOCK_START;
					vowel += (vowel >= 2);
				}
				else {
					/* Omit unrecognized characters. */
					continue;
				}

				/* Account for irregular consonant-vowel combinations. */
				if(cons == CONS_S && vowel == VOWEL_I) {
					cons = CONS_SH; // shi
				}
				else if(cons == CONS_T) {
					if(vowel == VOWEL_I) cons = CONS_CH; // chi
					else if(vowel == VOWEL_U) cons = CONS_TS; // tsu
				}
				else if(cons == CONS_H && vowel == VOWEL_U) {
					cons = CONS_F; // fu
				}
				else if(cons == CONS_Z && vowel == VOWEL_I) {
					cons = CONS_J; // ji
				}
				else if(cons == CONS_D) {
					if(vowel == VOWEL_I) cons = CONS_J; // ji (AKA dji, dzi)
					else if(vowel == VOWEL_U) cons = CONS_Z; // zu (AKA dzu)
				}
				else if(!is_katakana && cons == CONS_W && vowel == VOWEL_O) {
					cons = CONS_NONE; // hiragana wo
				}

				/* Handle small consonants such as glides ya, yu, yo and small wa. */
				if(is_small_cons) {
					pop_vowel();
					if(prev_cons == CONS_NONE) {
						/* Note that prev_cons is re-assigned to fool uses_glide()
						into treating certain vowels as consonant sounds. */
						romaji += cons_to_romaji(prev_cons = vowel_to_cons(prev_vowel));
					}
					if(uses_glide(prev_cons)) {
						romaji += cons_to_romaji(cons);
					}
				}
				else if(cons == SMALL_VOWEL) {
					pop_vowel();
					if(prev_cons == CONS_NONE) {
						romaji += cons_to_romaji(vowel_to_cons(prev_vowel));
					}
					else if(prev_vowel == VOWEL_I && vowel == VOWEL_E && uses_glide(prev_cons)) {
						romaji += 'y';
					}
					else if(prev_vowel == VOWEL_U && uses_w_glide(prev_cons)) {
						romaji += 'w';
					}
				}
				else if(cons != CONS_NONE) {
					if(was_small_tsu) {
						romaji += small_tsu_romaji(cons);
					}
					romaji += cons_to_romaji(cons);
				}
				romaji += vowel_to_romaji(vowel);

			}
		}
	}

	const char *kana;
	std::string &romaji;
	core::unicode_char curr;

};

}

void japanese::utf8_kana_to_romaji(const char *kana, std::string &romaji)
	throw(core::utf8_encoding_error) {
	utf8_kana_to_romaji_converter(kana, romaji).convert();
}


#ifndef _JAPANESE_ROMAJI_H_
#define _JAPANESE_ROMAJI_H_

#include "utf8.h"

namespace japanese {

/* Read a null-terminated UTF-8 string in kana and write the equivalent romaji
spelling to the string romaji. Only throws an exception if the UTF-8 encoding
was incorrect. Unrecognizable characters are omitted. */
void utf8_kana_to_romaji(const char *kana, std::string &romaji)
	throw(core::utf8_encoding_error);

} // namespace japanese

#endif


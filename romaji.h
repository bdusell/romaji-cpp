#include "utf8.h"

namespace japanese {

/* Read a null-terminated UTF-8 string in kana and write the equivalent romaji
spelling to the string romaji. Throws no exceptions. Unrecognizable characters
are omitted. */
void utf8_kana_to_romaji(const char *kana, std::string &romaji);

} // namespace japanese


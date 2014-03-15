#ifndef _CORE_UTF_H_
#define _CORE_UTF_H_

#include <stdint.h>
#include <stdexcept>

namespace core {

/* An integral type suitable for encoding a Unicode character. */
typedef uint_least32_t unicode_char;

struct utf8_encoding_error;

/* An exception indicating that a UTF-8 conversion failed. */
struct utf8_encoding_error : public std::invalid_argument {
	inline utf8_encoding_error(const std::string &msg);
};

/* Read the next UTF-8 character encoded in the null-terminated string input
and return the address of the next byte after it. */
const char *next_utf8_char(const char *input)
	throw (utf8_encoding_error);

/* Read the next UTF-8 character from input as in next_utf8_char, and write the
encoded value to out. Return the address of the following byte. */
const char *read_utf8_char(const char *input, unicode_char &out)
	throw (utf8_encoding_error);

} // namespace core

namespace core {

utf8_encoding_error::utf8_encoding_error(const std::string &msg)
	: std::invalid_argument(msg) {}

} // namespace core

#endif


#include "utf8.h"

namespace core {

const char *next_utf8_char(const char *input)
	throw (utf8_encoding_error)
{
	char head = *input++;
	/* If the first bit is unset, then it's just an ASCII character. */
	if(head & 0x80) {
		/* If the first bit is set, for every following set bit, read a
		trailing byte. */
		unsigned char n = 0;
		while((head <<= 1) & 0x80) {
			if(((*input) & 0xc0) != 0x80) {
				throw utf8_encoding_error("trailing byte does not start with the bits \'10\'");
			}
			++input;
			++n;
		}
		if(n < 0) throw utf8_encoding_error("too few bits set in leading byte (only 1)");
		if(n > 5) throw utf8_encoding_error("too many bits set in leading byte (more than 6)");
		
	}
	return input;
}

const char *read_utf8_char(const char *input, unicode_char &out)
	throw (utf8_encoding_error)
{

	const char *end = next_utf8_char(input);
	unsigned char size = end - input;
	char head = *input++;

	out = 0;

	if(size == 1) {
		out = head;
	}
	else {
		/* Get the bits following the first unset bit. */
		out |= head & 0xf;

		/* Use the last 6 bits of the trailing bytes. */
		for(const char *i = input; i != end; ++i) {
			char curr = *i;
			out <<= 6;
			out |= curr & 0x3f;
		}

		/* Check that the shortest possible multi-byte sequence was used. */
		/*
		Lower limit (hex)  Bit from right   Expression for previous column
		0x80               8                8 + 4(0) + (0)
		0x800              12               8 + 4(1) + (0)
		0x10000            17               8 + 4(2) + (1)
		0x200000           22               8 + 4(3) + (2)
		0x4000000          27               8 + 4(4) + (3)
		*/
		unsigned char n = size - 2;
		unicode_char lower_limit = (1 << (8 + 4 * n + (n > 1 ? n - 1 : 0) - 1));
		if(out < lower_limit) {
			throw utf8_encoding_error("shortest possible multi-byte sequence was not used");
		}

	}

	return end;

}

} // namespace core


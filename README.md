romaji-cpp
==========

This is a small C++ library and accompanying driver program which
transliterates UTF-8 strings of kana (Japanese phonetic characters) into romaji
(Latin letters). Special care was taken to handle all variety of inputs,
including some exotic edge cases.

The driver program `kana_to_romaji` is a command-line tool which converts its
argument from a UTF-8 encoded string of kana into romaji on `stdout`. It is built
with

	make

or

	make kana_to_romaji

To build the driver program and perform unit tests, use

	make test

Example Usage
-------------

Simple cases work as is to be expected.

	./kana_to_romaji ひらがな
	hiragana

	./kana_to_romaji カタカナ
	katakana

	./kana_to_romaji はっぴょう
	happyou

	./kana_to_romaji つくって
	tsukutte

	./kana_to_romaji ふじさん
	fujisan

	./kana_to_romaji ぎゅうにゅう
	gyuunyuu

	./kana_to_romaji じんじゃ
	jinja

Special consonant-vowel combinations which are not common to native Japanese
words work as well.

	./kana_to_romaji パーティー
	paatii

	./kana_to_romaji タトゥー
	tatuu

	./kana_to_romaji クァルテット
	kwarutetto

Perhaps one of the more interesting cases is the various spellings of "Whiskey."

	./kana_to_romaji ウィスキー
	wisukii

	./kana_to_romaji ウイスキー
	uisukii

	./kana_to_romaji ウヰスキー
	uwisukii


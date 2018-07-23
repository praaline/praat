/* melder_writetext.cpp
 *
 * Copyright (C) 2007,2008,2010-2013,2015-2018 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "melder.h"
#include "Preferences.h"
#include "UnicodeData.h"
#include "abcio.h"

void Melder_fwrite32to8 (conststring32 string, FILE *f) {
	/*
	 * Precondition:
	 *    the string's encoding is UTF-32.
	 * Failure:
	 *    if the precondition does not hold, we don't crash,
	 *    but the characters that are written may be incorrect.
	 */
	for (const char32* p = string; *p != U'\0'; p ++) {
		char32 kar = *p;
		if (kar <= 0x00007F) {
			#ifdef _WIN32
				if (kar == U'\n') fputc (13, f);
			#endif
			fputc ((int) kar, f);   // because fputc wants an int instead of an uint8 (guarded conversion)
		} else if (kar <= 0x0007FF) {
			fputc (0xC0 | (kar >> 6), f);
			fputc (0x80 | (kar & 0x00003F), f);
		} else if (kar <= 0x00FFFF) {
			fputc (0xE0 | (kar >> 12), f);
			fputc (0x80 | ((kar >> 6) & 0x00003F), f);
			fputc (0x80 | (kar & 0x00003F), f);
		} else {
			fputc (0xF0 | (kar >> 18), f);
			fputc (0x80 | ((kar >> 12) & 0x00003F), f);
			fputc (0x80 | ((kar >> 6) & 0x00003F), f);
			fputc (0x80 | (kar & 0x00003F), f);
		}
	}
}

void MelderFile_writeText (MelderFile file, conststring32 text, kMelder_textOutputEncoding outputEncoding) {
	if (! text) text = U"";
	autofile f = Melder_fopen (file, "wb");
	if (outputEncoding == kMelder_textOutputEncoding::UTF8) {
		Melder_fwrite32to8 (text, f);
	} else if ((outputEncoding == kMelder_textOutputEncoding::ASCII_THEN_UTF16 && Melder_isValidAscii (text)) ||
		(outputEncoding == kMelder_textOutputEncoding::ISO_LATIN1_THEN_UTF16 && Melder_isEncodable (text, kMelder_textOutputEncoding_ISO_LATIN1)))
	{
		#ifdef _WIN32
			#define flockfile(f)  (void) 0
			#define funlockfile(f)  (void) 0
			#define putc_unlocked  putc
		#endif
		flockfile (f);
		size_t n = str32len (text);
		for (size_t i = 0; i < n; i ++) {
			char32 kar = text [i];
			#ifdef _WIN32
				if (kar == U'\n') putc_unlocked (13, f);
			#endif
			putc_unlocked (kar, f);
		}
		funlockfile (f);
	} else {
		binputu16 (0xFEFF, f);   // Byte Order Mark
		size_t n = str32len (text);
		for (size_t i = 0; i < n; i ++) {
			char32 kar = text [i];
			#ifdef _WIN32
				if (kar == U'\n') binputu16 (13, f);
			#endif
			if (kar <= 0x00FFFF) {
				binputu16 ((char16) kar, f);   // guarded conversion down
			} else if (kar <= 0x10FFFF) {
				kar -= 0x010000;
				binputu16 (0xD800 | (uint16) (kar >> 10), f);
				binputu16 (0xDC00 | (uint16) ((char16) kar & 0x3ff), f);
			} else {
				binputu16 (UNICODE_REPLACEMENT_CHARACTER, f);
			}
		}
	}
	f.close (file);
}

void MelderFile_appendText (MelderFile file, conststring32 text) {
	if (! text) text = U"";
	autofile f1;
	try {
		f1.reset (Melder_fopen (file, "rb"));
	} catch (MelderError) {
		Melder_clearError ();   // it's OK if the file didn't exist yet...
		MelderFile_writeText (file, text, Melder_getOutputEncoding ());   // because then we just "write"
		return;
	}
	/*
	 * The file already exists and is open. Determine its type.
	 */
	int firstByte = fgetc (f1), secondByte = fgetc (f1);
	f1.close (file);
	int type = 0;
	if (firstByte == 0xfe && secondByte == 0xff) {
		type = 1;   // big-endian 16-bit
	} else if (firstByte == 0xff && secondByte == 0xfe) {
		type = 2;   // little-endian 16-bit
	}
	if (type == 0) {
		kMelder_textOutputEncoding outputEncoding = Melder_getOutputEncoding ();
		if (outputEncoding == kMelder_textOutputEncoding::UTF8) {   // TODO: read as file's encoding
			autofile f2 = Melder_fopen (file, "ab");
			Melder_fwrite32to8 (text, f2);
			f2.close (file);
		} else if ((outputEncoding == kMelder_textOutputEncoding::ASCII_THEN_UTF16 && Melder_isEncodable (text, kMelder_textOutputEncoding_ASCII))
		    || (outputEncoding == kMelder_textOutputEncoding::ISO_LATIN1_THEN_UTF16 && Melder_isEncodable (text, kMelder_textOutputEncoding_ISO_LATIN1)))
		{
			/*
			 * Append ASCII or ISOLatin1 text to ASCII or ISOLatin1 file.
			 */
			autofile f2 = Melder_fopen (file, "ab");
			int64 n = str32len (text);
			for (int64 i = 0; i < n; i ++) {
				char32 kar = text [i];
				#ifdef _WIN32
					if (kar == U'\n') fputc (13, f2);
				#endif
				fputc ((char8) kar, f2);
			}
			f2.close (file);
		} else {
			/*
			 * Convert to wide character file.
			 */
			autostring32 oldText = MelderFile_readText (file);
			autofile f2 = Melder_fopen (file, "wb");
			binputu16 (0xfeff, f2);
			int64 n = str32len (oldText.get());
			for (int64 i = 0; i < n; i ++) {
				char32 kar = oldText [i];
				#ifdef _WIN32
					if (kar == U'\n') binputu16 (13, f2);
				#endif
				if (kar <= 0x00FFFF) {
					binputu16 ((uint16) kar, f2);   // guarded conversion down
				} else if (kar <= 0x10FFFF) {
					kar -= 0x010000;
					binputu16 ((uint16) (0x00D800 | (kar >> 10)), f2);
					binputu16 ((uint16) (0x00DC00 | (kar & 0x0003ff)), f2);
				} else {
					binputu16 (UNICODE_REPLACEMENT_CHARACTER, f2);
				}
			}
			n = str32len (text);
			for (int64 i = 0; i < n; i ++) {
				char32 kar = text [i];
				#ifdef _WIN32
					if (kar == U'\n') binputu16 (13, f2);
				#endif
				if (kar <= 0x00FFFF) {
					binputu16 ((uint16) kar, f2);   // guarded conversion down
				} else if (kar <= 0x10FFFF) {
					kar -= 0x010000;
					binputu16 ((uint16) (0x00D800 | (kar >> 10)), f2);
					binputu16 ((uint16) (0x00DC00 | (kar & 0x0003ff)), f2);
				} else {
					binputu16 (UNICODE_REPLACEMENT_CHARACTER, f2);
				}
			}
			f2.close (file);
		}
	} else {
		autofile f2 = Melder_fopen (file, "ab");
		int64 n = str32len (text);
		for (int64 i = 0; i < n; i ++) {
			if (type == 1) {
				char32 kar = text [i];
				#ifdef _WIN32
					if (kar == U'\n') binputu16 (13, f2);
				#endif
				if (kar <= 0x00FFFF) {
					binputu16 ((uint16) kar, f2);   // guarded conversion down
				} else if (kar <= 0x10FFFF) {
					kar -= 0x010000;
					binputu16 ((uint16) (0x00D800 | (kar >> 10)), f2);
					binputu16 ((uint16) (0x00DC00 | (kar & 0x0003ff)), f2);
				} else {
					binputu16 (UNICODE_REPLACEMENT_CHARACTER, f2);
				}
			} else {
				char32 kar = text [i];
				#ifdef _WIN32
					if (kar == U'\n') binputu16LE (13, f2);
				#endif
				if (kar <= 0x00FFFF) {
					binputu16LE ((uint16) kar, f2);   // guarded conversion down
				} else if (kar <= 0x10FFFF) {
					kar -= 0x010000;
					binputu16LE ((uint16) (0x00D800 | (kar >> 10)), f2);
					binputu16LE ((uint16) (0x00DC00 | (kar & 0x0003ff)), f2);
				} else {
					binputu16LE (UNICODE_REPLACEMENT_CHARACTER, f2);
				}
			}
		}
		f2.close (file);
	}
}

void _MelderFile_write (MelderFile file, conststring32 string) {
	if (! string) return;
	int64 length = str32len (string);
	FILE *f = file -> filePointer;
	if (file -> outputEncoding == kMelder_textOutputEncoding_ASCII || file -> outputEncoding == kMelder_textOutputEncoding_ISO_LATIN1) {
		for (int64 i = 0; i < length; i ++) {
			char kar = (char) (char8) string [i];   // truncate
			if (kar == '\n' && file -> requiresCRLF) putc (13, f);
			putc (kar, f);
		}
	} else if (file -> outputEncoding == (unsigned long) kMelder_textOutputEncoding::UTF8) {
		for (int64 i = 0; i < length; i ++) {
			char32 kar = string [i];
			if (kar <= 0x00007F) {
				if (kar == U'\n' && file -> requiresCRLF) putc (13, f);
				putc ((int) kar, f);   // guarded conversion down
			} else if (kar <= 0x0007FF) {
				putc (0xC0 | (kar >> 6), f);
				putc (0x80 | (kar & 0x00003F), f);
			} else if (kar <= 0x00FFFF) {
				putc (0xE0 | (kar >> 12), f);
				putc (0x80 | ((kar >> 6) & 0x00003F), f);
				putc (0x80 | (kar & 0x00003F), f);
			} else {
				putc (0xF0 | (kar >> 18), f);
				putc (0x80 | ((kar >> 12) & 0x00003F), f);
				putc (0x80 | ((kar >> 6) & 0x00003F), f);
				putc (0x80 | (kar & 0x00003F), f);
			}
		}
	} else {
		for (int64 i = 0; i < length; i ++) {
			char32 kar = string [i];
			if (kar == U'\n' && file -> requiresCRLF) binputu16 (13, f);
			if (kar <= 0x00FFFF) {
				binputu16 ((char16) kar, f);
			} else if (kar <= 0x10FFFF) {
				kar -= 0x010000;
				binputu16 (0xD800 | (char16) (kar >> 10), f);
				binputu16 (0xDC00 | (char16) ((char16) kar & 0x03ff), f);
			} else {
				binputu16 (UNICODE_REPLACEMENT_CHARACTER, f);
			}
		}
	}
}

void MelderFile_writeCharacter (MelderFile file, char32 kar) {
	FILE *f = file -> filePointer;
	if (file -> outputEncoding == kMelder_textOutputEncoding_ASCII || file -> outputEncoding == kMelder_textOutputEncoding_ISO_LATIN1) {
		if (kar == U'\n' && file -> requiresCRLF) putc (13, f);
		putc ((int) kar, f);
	} else if (file -> outputEncoding == (unsigned long) kMelder_textOutputEncoding::UTF8) {
		if (kar <= 0x00007F) {
			if (kar == U'\n' && file -> requiresCRLF) putc (13, f);
			putc ((int) kar, f);   // guarded conversion down
		} else if (kar <= 0x0007FF) {
			putc (0xC0 | (kar >> 6), f);
			putc (0x80 | (kar & 0x00003F), f);
		} else if (kar <= 0x00FFFF) {
			putc (0xE0 | (kar >> 12), f);
			putc (0x80 | ((kar >> 6) & 0x00003F), f);
			putc (0x80 | (kar & 0x00003F), f);
		} else {
			putc (0xF0 | (kar >> 18), f);
			putc (0x80 | ((kar >> 12) & 0x00003F), f);
			putc (0x80 | ((kar >> 6) & 0x00003F), f);
			putc (0x80 | (kar & 0x00003F), f);
		}
	} else {
		if (kar == U'\n' && file -> requiresCRLF) binputu16 (13, f);
		if (kar <= 0x00FFFF) {
			binputu16 ((uint16) kar, f);
		} else if (kar <= 0x10FFFF) {
			kar -= 0x010000;
			binputu16 (0xD800 | (uint16) (kar >> 10), f);
			binputu16 (0xDC00 | (uint16) ((uint16) kar & 0x0003ff), f);
		} else {
			binputu16 (UNICODE_REPLACEMENT_CHARACTER, f);
		}
	}
}

/* End of file melder_writetext.cpp */

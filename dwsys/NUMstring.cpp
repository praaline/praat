/* NUMstring.cpp
 *
 * Copyright (C) 2012-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20121005 First version
*/

#include <ctype.h>
#include <wctype.h>
#include "Interpreter.h"
#include "NUM2.h"

bool NUMstring_containsPrintableCharacter (conststring32 s) {
	if (! s) return false;
	integer len = str32len (s);
	if (len == 0) return false;
	for (integer i = 0; i < len; i ++) {
		if (isgraph ((int) s [i]))
			return true;
	}
	return false;
}

void NUMstring_chopWhiteSpaceAtExtremes_inplace (char32 *string) {
	int64 start = 0;
	while (Melder_isHorizontalOrVerticalSpace (string [start])) {
		start ++;
	}
	int64 end = str32len (string);
	while (end != start && Melder_isHorizontalOrVerticalSpace (string [end - 1])) {
		end --;
	}
	int64 n = end - start;
	memmove (string, string + start, (size_t) (n * (int64) sizeof (char32)));
	string [n] = 0;
}

double *NUMstring_to_numbers (conststring32 s, integer *p_numbers_found) {
	integer numbers_found = Melder_countTokens (s);
	if (numbers_found < 1) {
		Melder_throw (U"Empty string.");
	}
	autoNUMvector <double> numbers (1, numbers_found);
	integer inum = 1;
	for (char32 *token = Melder_firstToken (s); token; token = Melder_nextToken (), inum ++) {
		Interpreter_numericExpression (0, token, & numbers [inum]);
	}
	if (p_numbers_found) {
		*p_numbers_found = numbers_found;
	}
	return numbers.transfer();
}

char32 *strstr_regexp (conststring32 string, conststring32 search_regexp) {
	char32 *charp = nullptr;
	regexp *compiled_regexp = CompileRE_throwable (search_regexp, 0);

	if (ExecRE (compiled_regexp, nullptr, string, nullptr, false, U'\0', U'\0', nullptr, nullptr, nullptr)) {
		charp = compiled_regexp -> startp [0];
	}

	free (compiled_regexp);
	return charp;
}

autostring32 str_replace_literal (conststring32 string,
	conststring32 search, conststring32 replace, integer maximumNumberOfReplaces,
	integer *nmatches)
{
	if (string == 0 || search == 0 || replace == 0)
		return autostring32();

	integer len_string = str32len (string);
	if (len_string == 0)
		maximumNumberOfReplaces = 1;

	integer len_search = str32len (search);
	if (len_search == 0)
		maximumNumberOfReplaces = 1;

	/*
		To allocate memory for 'result' only once, we have to know how many
		matches will occur.
	*/

	const char32 *pos = & string [0];   // current position / start of current match
	*nmatches = 0;
	if (maximumNumberOfReplaces <= 0)
		maximumNumberOfReplaces = INTEGER_MAX;

	if (len_search == 0) { /* Search is empty string... */
		if (len_string == 0)
			*nmatches = 1;    /* ...only matches empty string */
	} else {
		if (len_string != 0) { /* Because empty string always matches */
			while (!! (pos = str32str (pos, search)) && *nmatches < maximumNumberOfReplaces) {
				pos += len_search;
				(*nmatches) ++;
			}
		}
	}

	integer len_replace = str32len (replace);
	integer len_result = len_string + *nmatches * (len_replace - len_search);
	autostring32 result (len_result);

	const char32 *posp = pos = & string [0];
	integer nchar = 0, result_nchar = 0;
	for (integer i = 1; i <= *nmatches; i ++) {
		pos = str32str (pos, search);

		/*
			Copy gap between end of previous match and start of current.
		*/
		nchar = (pos - posp);
		if (nchar > 0) {
			str32ncpy (& result [result_nchar], posp, nchar);
			result_nchar += nchar;
		}

		/*
			Insert the replace string in result.
		*/
		str32ncpy (& result [result_nchar], replace, len_replace);
		result_nchar += len_replace;

		/*
			Next search starts after the match.
		*/
		pos += len_search;
		posp = pos;
	}

	/*
		Copy gap between end of match and end of string.
	*/
	pos = string + len_string;
	nchar = pos - posp;
	if (nchar > 0)
		str32ncpy (& result [result_nchar], posp, nchar);
	return result;
}

autostring32 str_replace_regexp (conststring32 string,
	regexp *compiledSearchRE, conststring32 replaceRE, integer maximumNumberOfReplaces,
	integer *nmatches)
{
	integer buf_nchar = 0;   // number of characters in 'buf'
	integer gap_copied = 0;
	integer nchar;
	bool reverse = false;
	int errorType;
	char32 prev_char = U'\0';
	const char32 *pos;   // current position in 'string' / start of current match
	const char32 *posp;   // end of previous match
	autostring32 buf;

	*nmatches = 0;
	if (string == 0 || compiledSearchRE == 0 || replaceRE == 0)
		return 0;

	integer string_length = str32len (string);
	//int replace_length = str32len (replaceRE);
	if (string_length == 0)
		maximumNumberOfReplaces = 1;

	integer i = maximumNumberOfReplaces > 0 ? 0 : - string_length;

	/*
		We do not know the size of the replaced string in advance,
		therefor, we allocate a replace buffer twice the size of the
		original string. After all replaces have taken place we do a
		final realloc to the then exactly known size.
		If during the replace, the size of the buffer happens to be too
		small (this is signalled by the replaceRE function),
		we double its size and restart the replace.
	*/

	integer bufferLength = 2 * string_length;
	bufferLength = bufferLength < 100 ? 100 : bufferLength;
	buf.resize (bufferLength);

	pos = posp = string;
	while (ExecRE (compiledSearchRE, nullptr, pos, nullptr, reverse, prev_char, U'\0', nullptr, nullptr, nullptr) && i ++ < maximumNumberOfReplaces) {
		/*
			Copy gap between the end of the previous match and the start
			of the current match.
			Check buffer overflow. pos == posp ? '\0' : pos [-1],
		*/
		pos = compiledSearchRE -> startp [0];
		nchar = pos - posp;
		if (nchar > 0 && ! gap_copied) {
			if (buf_nchar + nchar > bufferLength) {
				bufferLength *= 2;
				buf.resize (bufferLength);
			}
			str32ncpy (buf.get() + buf_nchar, posp, nchar);
			buf_nchar += nchar;
		}

		gap_copied = 1;

		/*
			Do the substitution. We can only check afterwards for buffer
			overflow. SubstituteRE puts null byte at last replaced position and signals when overflow.
		*/
		if (! SubstituteRE (compiledSearchRE, replaceRE, buf.get() + buf_nchar, bufferLength + 1 - buf_nchar, & errorType)) {
			if (errorType == 1) {   // not enough memory
				bufferLength *= 2;
				buf.resize (bufferLength);
				Melder_clearError ();
				i --;   // retry
				continue;
			}
			Melder_throw (U"Error during substitution.");
		}

		// Buffer is not full, get number of characters added;

		nchar = str32len (buf.get() + buf_nchar);
		buf_nchar += nchar;

		// Update next start position in search string.

		posp = pos;
		pos = (char32 *) compiledSearchRE -> endp [0];
		if (pos != posp)
			prev_char = pos [-1];
		gap_copied = 0;
		posp = pos; //pb 20080121
		(*nmatches) ++;
		// at end of string?
		// we need this because .* matches at end of a string
		if (pos - string == string_length)
			break;
	}

	// Copy last part of string to destination string

	nchar = (string + string_length) - pos;
	bufferLength = buf_nchar + nchar;
	buf.resize (bufferLength);
	str32ncpy (buf.get() + buf_nchar, pos, nchar);
	return buf;
}

static autostring32vector string32vector_searchAndReplace_literal (string32vector me,
	conststring32 search, conststring32 replace, int maximumNumberOfReplaces,
	integer *out_numberOfMatches, integer *out_numberOfStringMatches)
{
	if (! search || ! replace)
		return autostring32vector();
	autostring32vector result (me.size);

	integer nmatches_sub = 0, nmatches = 0, nstringmatches = 0;
	for (integer i = 1; i <= me.size; i ++) {
		conststring32 string = ( me [i] ? me [i] : U"" );   // treat null as an empty string

		result [i] = str_replace_literal (string, search, replace, maximumNumberOfReplaces, & nmatches_sub);
		if (nmatches_sub > 0) {
			nmatches += nmatches_sub;
			nstringmatches ++;
		}
	}
	if (out_numberOfMatches)
		*out_numberOfMatches = nmatches;
	if (out_numberOfStringMatches)
		*out_numberOfStringMatches = nstringmatches;
	return result;
}

static autostring32vector string32vector_searchAndReplace_regexp (string32vector me,
	conststring32 searchRE, conststring32 replaceRE, int maximumNumberOfReplaces,
	integer *out_numberOfMatches, integer *out_numberOfStringMatches)
{
	if (! searchRE || ! replaceRE)
		return autostring32vector();

	integer nmatches_sub = 0;

	regexp *compiledRE = CompileRE_throwable (searchRE, 0);

	autostring32vector result (me.size);

	integer nmatches = 0, nstringmatches = 0;
	for (integer i = 1; i <= me.size; i ++) {
		conststring32 string = ( me [i] ? me [i] : U"" );   // treat null as an empty string
		result [i] = str_replace_regexp (string, compiledRE, replaceRE, maximumNumberOfReplaces, & nmatches_sub);
		if (nmatches_sub > 0) {
			nmatches += nmatches_sub;
			nstringmatches ++;
		}
	}
	if (out_numberOfMatches)
		*out_numberOfMatches = nmatches;
	if (out_numberOfStringMatches)
		*out_numberOfStringMatches = nstringmatches;
	return result;
}

autostring32vector string32vector_searchAndReplace (string32vector me,
	conststring32 search, conststring32 replace, int maximumNumberOfReplaces,
	integer *nmatches, integer *nstringmatches, bool use_regexp)
{
	return use_regexp ?
		string32vector_searchAndReplace_regexp (me, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches) :
		string32vector_searchAndReplace_literal (me, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches);
}

/*
 * Acceptable ranges e.g. "1 4 2 3:7 4:3 3:5:2" -->
 * 1, 4, 2, 3, 4, 5, 6, 7, 4, 3, 3, 4, 5, 4, 3, 2
 * Overlap is allowed. Ranges can go up and down.
 */
static integer *getElementsOfRanges (conststring32 ranges, integer maximumElement, integer *numberOfElements, conststring32 elementType) {
	/*
		Count the elements.
	*/
	integer previousElement = 0;
	*numberOfElements = 0;
	const char32 *p = & ranges [0];
	for (;;) {
		while (Melder_isHorizontalSpace (*p)) p ++;
		if (*p == U'\0')
			break;
		if (Melder_isAsciiDecimalNumber (*p)) {
			integer currentElement = Melder_atoi (p);
			Melder_require (currentElement != 0,
				U"No such ", elementType, U": 0 (minimum is 1).");
			Melder_require (currentElement <= maximumElement,
				U"No such ", elementType, U": ", currentElement, U" (maximum is ", maximumElement, U").");
			
			*numberOfElements += 1;
			previousElement = currentElement;
			do { p ++; } while (Melder_isAsciiDecimalNumber (*p));
		} else if (*p == ':') {
			Melder_require (previousElement != 0, U"The range should not start with a colon.");
			
			do { p ++; } while (Melder_isHorizontalSpace (*p));
			Melder_require (*p != U'\0',
				U"The range should not end with a colon.");
			Melder_require (Melder_isAsciiDecimalNumber (*p),
				U"End of range should be a positive whole number.");
			
			integer currentElement = Melder_atoi (p);
			Melder_require (currentElement != 0,
				U"No such ", elementType, U": 0 (minimum is 1).");
			Melder_require (currentElement <= maximumElement,
				U"No such ", elementType, U": ", currentElement, U" (maximum is ", maximumElement, U").");
			
			if (currentElement > previousElement) {
				*numberOfElements += currentElement - previousElement;
			} else {
				*numberOfElements += previousElement - currentElement;
			}
			previousElement = currentElement;
			do { p ++; } while (Melder_isAsciiDecimalNumber (*p));
		} else {
			Melder_throw (U"Start of range should be a positive whole number.");
		}
	}

	/*
		Create room for the elements.
	*/
	if (*numberOfElements == 0)
		return nullptr;
	autoNUMvector <integer> elements (1, *numberOfElements);
	
	/*
		Store the elements.
	*/
	previousElement = 0;
	*numberOfElements = 0;
	p = & ranges [0];
	for (;;) {
		while (Melder_isHorizontalSpace (*p)) p ++;
		if (*p == U'\0')
			break;
		if (Melder_isAsciiDecimalNumber (*p)) {
			integer currentElement = Melder_atoi (p);
			elements [++ *numberOfElements] = currentElement;
			previousElement = currentElement;
			do { p ++; } while (Melder_isAsciiDecimalNumber (*p));
		} else if (*p == U':') {
			do { p ++; } while (Melder_isHorizontalSpace (*p));
			integer currentElement = Melder_atoi (p);
			if (currentElement > previousElement) {
				for (integer ielement = previousElement + 1; ielement <= currentElement; ielement ++)
					elements [++ *numberOfElements] = ielement;
			} else {
				for (integer ielement = previousElement - 1; ielement >= currentElement; ielement --)
					elements [++ *numberOfElements] = ielement;
			}
			previousElement = currentElement;
			do { p ++; } while (Melder_isAsciiDecimalNumber (*p));
		}
	}
	return elements.transfer();
}

static void NUMlvector_getUniqueNumbers (integer numbers[], integer *inout_numberOfElements, integer *out_numberOfMultiples) {
	Melder_assert (inout_numberOfElements);
	autoNUMvector< integer> sorted (NUMvector_copy <integer> (numbers, 1, *inout_numberOfElements), 1);
	NUMsort_integer (*inout_numberOfElements, sorted.peek());
	integer numberOfMultiples = 0;
	
	numbers [1] = sorted [1];
	integer numberOfUniques = 1;
	for (integer i = 2; i <= *inout_numberOfElements; i ++) {
		if (sorted [i] != sorted [i - 1]) {
			numbers [++ numberOfUniques] = sorted [i];
		} else {
			numberOfMultiples ++;
		}
	}
	*inout_numberOfElements = numberOfUniques;
	if (out_numberOfMultiples)
		*out_numberOfMultiples = numberOfMultiples;
}

integer *NUMstring_getElementsOfRanges (conststring32 ranges, integer maximumElement,
	integer *numberOfElements, integer *numberOfMultiples, conststring32 elementType, bool sortedUniques)
{
	autoNUMvector<integer> elements (getElementsOfRanges (ranges, maximumElement, numberOfElements, elementType), 1);
	if (sortedUniques && *numberOfElements > 0)
		NUMlvector_getUniqueNumbers (elements.peek(), numberOfElements, numberOfMultiples);
	return elements.transfer();
}

char32 * NUMstring_timeNoDot (double time) {
	static char32 string [100];
	integer seconds = Melder_ifloor (time);
	integer ms = Melder_iround ((time - seconds) * 1000.0);
	Melder_sprint (string,100, U"_", seconds, U"_", ms);
	return string;
}

/* End of file NUMstring.cpp */

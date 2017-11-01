/* SpeechSynthesizer_def.h
 *
 * Copyright (C) 2011-2012, 2015-2016 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#define ooSTRUCT SpeechSynthesizerVoice
oo_DEFINE_CLASS (SpeechSynthesizerVoice, Daata)
	oo_STRING (d_v_name)

	oo_INTEGER (d_phoneme_tab_ix)  // phoneme table number
	oo_INTEGER (d_pitch_base)    // Hz
	oo_INTEGER (d_pitch_range)   // Hz

	oo_INTEGER (d_speedf1)
	oo_INTEGER (d_speedf2)
	oo_INTEGER (d_speedf3)

	oo_DOUBLE (d_speed_percent)      // adjust the WPM speed by this percentage
	oo_DOUBLE (d_flutter)
	oo_DOUBLE (d_roughness)
	oo_DOUBLE (d_echo_delay)
	oo_DOUBLE (d_echo_amp)
	oo_INTEGER (d_n_harmonic_peaks)  // highest formant which is formed from adding harmonics
	oo_INT (d_peak_shape)        // alternative shape for formant peaks (0=standard 1=squarer)
	oo_DOUBLE (d_voicing)           // 100% = 64, level of formant-synthesized sound
	oo_DOUBLE (d_formant_factor)      // adjust nominal formant frequencies by this  because of the voice's pitch (256ths)
	oo_DOUBLE (d_consonant_amp)     // amplitude of unvoiced consonants
	oo_DOUBLE (d_consonant_ampv)    // amplitude of the noise component of voiced consonants
	oo_DOUBLE (d_samplingFrequency)
	oo_INT_VECTOR_FROM (d_klattv, 0, 7)

	// parameters used by Wavegen
	oo_INTEGER (d_numberOfFormants) // < 8
	oo_INT_VECTOR_FROM (d_freq, 0, d_numberOfFormants)		// 100% = 256
	oo_INT_VECTOR_FROM (d_height, 0, d_numberOfFormants)	// 100% = 256
	oo_INT_VECTOR_FROM (d_width, 0, d_numberOfFormants)		// 100% = 256
	oo_INT_VECTOR_FROM (d_freqadd, 0, d_numberOfFormants)	// Hz

	// copies without temporary adjustments from embedded commands
	oo_INT_VECTOR_FROM (d_freq2, 0, d_numberOfFormants)		// 100% = 256
	oo_INT_VECTOR_FROM (d_height2, 0, d_numberOfFormants)	// 100% = 256
	oo_INT_VECTOR_FROM (d_width2, 0, d_numberOfFormants)	// 100% = 256

	oo_INT_VECTOR_FROM (d_breath, 0, d_numberOfFormants)	// amount of breath for each formant. breath[0] indicates whether any are set.
	oo_INT_VECTOR_FROM (d_breathw, 0, d_numberOfFormants)	// width of each breath formant

oo_END_CLASS (SpeechSynthesizerVoice)
#undef ooSTRUCT


#define ooSTRUCT SpeechSynthesizer
oo_DEFINE_CLASS (SpeechSynthesizer, Daata)
	oo_FROM (1)
		oo_STRING (d_synthesizerVersion)
	oo_ENDFROM

	// sythesizers language /voice
	oo_STRING (d_languageName)
	oo_STRING (d_voiceName)
	oo_FROM (1)
		oo_STRING (d_phonemeSet)
	oo_ENDFROM
	#if oo_READING
		if (formatVersion < 1) {
			d_phonemeSet = Melder_dup (d_languageName);
			d_synthesizerVersion = Melder_dup (ESPEAK_NG_VERSION);
		}
	#endif
	oo_INTEGER (d_wordsPerMinute)
	// text-only, phonemes-only, mixed
	oo_INT (d_inputTextFormat)
	// 1/: output phonemes in espeak/ notation
	oo_INT (d_inputPhonemeCoding)
	// speech output
	oo_DOUBLE (d_samplingFrequency)
	oo_DOUBLE (d_wordgap)
	oo_DOUBLE (d_pitchAdjustment)
	oo_DOUBLE (d_pitchRange)
	// 1/2: output phonemes in espeak/IPA notation
	oo_INT (d_outputPhonemeCoding)

	#if oo_READING_TEXT
		if (formatVersion < 1) {
			oo_INT (d_estimateWordsPerMinute)   // this used to be oo_BOOL, which was written in text as 0 or 1, which is inappropriate for boolean text
		} else {
			oo_QUESTION (d_estimateWordsPerMinute)
		}
	#else
		oo_QUESTION (d_estimateWordsPerMinute)
	#endif

	#if !oo_READING && !oo_WRITING
		// Filled by the call back
		oo_AUTO_OBJECT (Table, 0, d_events)
		oo_DOUBLE (d_internalSamplingFrequency)
		oo_INTEGER (d_numberOfSamples)
		oo_INTEGER (d_wavCapacity)
		oo_INT_VECTOR (d_wav, d_wavCapacity)
	#endif
	#if oo_DECLARING
		void v_info () override;
	#endif

oo_END_CLASS (SpeechSynthesizer)
#undef ooSTRUCT

/* End of file SpeechSynthesizer_def.h */

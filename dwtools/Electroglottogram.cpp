/* Electroglottogram.cpp
 *
 * Copyright (C) 2019 David Weenink
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

#include "AmplitudeTier.h"
#include "Electroglottogram.h"
#include "Sound_and_Spectrum.h"
#include "Sound_extensions.h"
#include "Sound_to_PointProcess.h"

#include "enums_getText.h"
#include "Electroglottogram_enums.h"
#include "enums_getValue.h"
#include "Electroglottogram_enums.h"

Thing_implement (Electroglottogram, Sound, 2);

void IntervalTier_insertBoundary (IntervalTier me, double t) {
	try {
		Melder_require (! IntervalTier_hasTime (me, t),
			U"Cannot add a boundary at ", Melder_fixed (t, 6), U" seconds, because there is already a boundary there.");
		integer intervalNumber = IntervalTier_timeToIndex (me, t);
		Melder_require (intervalNumber != 0,
			U"Cannot add a boundary at ", Melder_fixed (t, 6), U" seconds, because this is outside the time domain of the intervals.");
		TextInterval interval = my intervals.at [intervalNumber];
		/*
			Move the text to the left of the boundary.
		*/
		autoTextInterval newInterval = TextInterval_create (t, interval -> xmax, U"");
		interval -> xmax = t;
		my intervals. addItem_move (newInterval.move());
	} catch (MelderError) {
		Melder_throw (me, U": boundary not inserted.");
	}
}

void IntervalTier_setIntervalText (IntervalTier me, integer intervalNumber, conststring32 text) {
	try {
		Melder_require (intervalNumber >= 1 && intervalNumber <= my intervals.size,
			U"Interval ", intervalNumber, U" does not exist.");
		TextInterval interval = my intervals.at [intervalNumber];
		TextInterval_setText (interval, text);
	} catch (MelderError) {
		Melder_throw (me, U": interval text not set.");
	}
}

void structElectroglottogram :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", our xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", our xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", our xmax - our xmin, U" seconds");
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of samples: ", our nx);
	MelderInfo_writeLine (U"   Sampling period: ", our dx, U" seconds");
	MelderInfo_writeLine (U"   Sampling frequency: ", Melder_single (1.0 / our dx), U" Hz");
	MelderInfo_writeLine (U"   First sample centred at: ", our x1, U" seconds");
}

autoElectroglottogram Electroglottogram_create (double xmin, double xmax, integer nx, double dx, double x1) {
	try {
		autoElectroglottogram me = Thing_new (Electroglottogram);
		Matrix_init (me.get(), xmin, xmax, nx, dx, x1, 1.0, 1.0, 1.0, 1, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Electroglottogram not created.");
	}
}

autoElectroglottogram Sound_extractElectroglottogram (Sound me, integer channel, bool invert) {
	try {
		Melder_require (channel > 0 && channel <= my ny,
			U"The channel number should be in the interval from 1 to ", my ny);
		autoElectroglottogram thee = Electroglottogram_create (my xmin, my xmax, my nx, my dx, my x1);
		thy z.all() <<= my z.row (channel);
		if (invert) 
			thy z.all() *= -1.0;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Electroglottogram.");
	}
}

autoAmplitudeTier Electroglottogram_to_AmplitudeTier_levels (Electroglottogram me, double pitchFloor, double pitchCeiling, double closingThreshold, autoAmplitudeTier *out_peaks, autoAmplitudeTier *out_valleys) {
	try {
		autoPointProcess peakPositions = Sound_to_PointProcess_periodic_peaks (me, pitchFloor, pitchCeiling, true, false);
		autoPointProcess valleyPositions = Sound_to_PointProcess_periodic_peaks (me, pitchFloor, pitchCeiling, false, true);
		/*
			Get the values of the peaks and valleys
		*/
		autoAmplitudeTier peaks = PointProcess_Sound_to_AmplitudeTier_point (peakPositions.get(), (Sound) me);
		autoAmplitudeTier valleys = PointProcess_Sound_to_AmplitudeTier_point (valleyPositions.get(), (Sound) me);
		autoAmplitudeTier thee = Electroglottogram_and_AmplitudeTiers_getLevels (me, peaks.get(), valleys.get(), closingThreshold);
		if (out_peaks)
			*out_peaks = peaks.move();
		if (out_valleys)
			*out_valleys = valleys.move();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": AmplitudeTier not created.");
	}
}

autoAmplitudeTier Electroglottogram_and_AmplitudeTiers_getLevels (Electroglottogram me, AmplitudeTier peaks, AmplitudeTier valleys, double closingThreshold) {
		try {
			Melder_require (my xmin == peaks -> xmin && my xmax == peaks -> xmax,
				U"The domains of the Electroglottogram and the peaks should be equal.");
			Melder_require (my xmin == valleys -> xmin && my xmax == valleys -> xmax,
				U"The domains of the Electroglottogram and the peaks should be equal.");
			Melder_require (peaks -> points. size > 1 && valleys -> points. size > 1,
				U"The AmplitudeTiers cannot be empty.");
			autoAmplitudeTier thee = AmplitudeTier_create (my xmin, my xmax);
			double peakAmplitudeLeft = RealTier_getValueAtIndex (peaks, 1);
			double peakTimeLeft = peaks -> points.at [1] -> number;
			for (integer ipoint = 2; ipoint <= peaks -> points. size - 1; ipoint ++) {
				double peakAmplitudeRight = RealTier_getValueAtIndex (peaks, ipoint);
				double peakTimeRight = peaks -> points.at [ipoint] -> number;
				integer indexValley = AnyTier_timeToNearestIndex ((AnyTier) valleys, peakTimeRight);
				double timeValley = valleys -> points.at [indexValley] -> number;
				if (timeValley > peakTimeLeft && timeValley < peakTimeRight) {
					double valleyAmplitude = RealTier_getValueAtIndex (valleys, indexValley);
					double amplitudeRange = peakAmplitudeLeft - valleyAmplitude;
					double level = valleyAmplitude + amplitudeRange * closingThreshold;
					RealTier_addPoint (thee.get(), peakTimeLeft, level);
				}
				peakAmplitudeLeft = peakAmplitudeRight;
				peakTimeLeft = peakTimeRight;
			}
			return thee;
		} catch (MelderError) {
			Melder_throw (me, U": Levels not determined.");
		}
}

autoIntervalTier Electroglottogram_getClosedGlottisIntervals (Electroglottogram me, double pitchFloor, double pitchCeiling, double closingThreshold, double silenceThreshold) {
	try {
		autoAmplitudeTier peaks, valleys;
		autoAmplitudeTier levels = Electroglottogram_to_AmplitudeTier_levels (me, pitchFloor, pitchCeiling, closingThreshold, & peaks,  & valleys);
		
		double minimum = RealTier_getMinimumValue (valleys.get());
		double maximum = RealTier_getMaximumValue (peaks.get());
		double minimumPeakAmplitude = maximum * silenceThreshold;

		autoIntervalTier intervalTier = IntervalTier_create (my xmin, my xmax);
		double previousOpeningTime = my xmin;
		for (integer ipoint = 1; ipoint <= peaks -> points. size; ipoint ++) {
			RealPoint peak = peaks -> points.at [ipoint];
			double peakPosition = peak -> number;
			double peakAmplitude = peak -> value;
			double closingTime = undefined, openingTime = undefined;
			if (peakAmplitude > minimumPeakAmplitude) {
				double level = RealTier_getValueAtTime (levels.get(), peakPosition);
				closingTime = Sound_getNearestLevelCrossing (me, 1, peakPosition, level, kSoundSearchDirection::Left);
				openingTime = Sound_getNearestLevelCrossing (me, 1, peakPosition, level, kSoundSearchDirection::Right);
				if (isdefined (closingTime) && isdefined (openingTime) && closingTime != previousOpeningTime) {
					IntervalTier_insertBoundary (intervalTier.get(), closingTime);
					IntervalTier_insertBoundary (intervalTier.get(), openingTime);
					double midPoint = 0.5 * (closingTime + openingTime);
					integer intervalNumber = IntervalTier_timeToIndex (intervalTier.get(), midPoint);
					IntervalTier_setIntervalText (intervalTier.get(), intervalNumber, U"c");
					previousOpeningTime = openingTime;
				}
			}
		}
		return intervalTier;
	} catch (MelderError) {
		Melder_throw (me, U": TextTier not created.");
	}
	
}

autoSound Electroglottogram_firstCentralDifference (Electroglottogram me, bool peak99) {
	try {	
		autoSound thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
		thy z [1] [1] = 0.0;
		for (integer i = 2; i < my nx; i ++)
			thy z [1] [i] = (my z [1] [i + 1] - my z [1] [i - 1]) / (2.0 * my dx);
			
		thy z [1] [my nx] = 0.0;
		if (peak99)
			Vector_scale (thee.get(), 0.99);
		return thee;
	} catch (MelderError) {
			Melder_throw (me, U": cannot create the simple derivative of the Electroglottogram.");
	}
}	

/*
	Derivative of x(t) = integral (X(f)exp(2*pi*f*t) * df)
	d(x(t)/dt = integral (X(f)*2*pi*i*exp(2*pi*f*t) * df)
			  = integral ((-2*pi*f*Im(X(f)), 2*pi*f*Re(X(f))) * exp (2*pi*f*t) * df)
*/
autoSound Electroglottogram_derivative (Electroglottogram me, double lowPassFrequency, double smoothing, bool peak99) {
		try {
			autoSpectrum thee = Sound_to_Spectrum (me, false);
			for (integer ifreq = 1; ifreq <= thy nx; ifreq ++) {
				double frequency = Sampled_indexToX (thee.get(), ifreq);
				double im = thy z [2] [ifreq];
				thy z [2] [ifreq] = 2.0 * NUMpi * frequency * thy z [1] [ifreq];
				thy z [1] [ifreq] = - 2.0 * NUMpi * frequency * im;
			}
			Spectrum_passHannBand (thee.get(), 0.0, lowPassFrequency, smoothing);
			autoSound him = Spectrum_to_Sound (thee.get());
			if (peak99)
				Vector_scale (him.get(), 0.99);
			return him;
		} catch (MelderError) {
			Melder_throw (me, U": cannot create the derivative of the Electroglottogram.");
		}
}

autoElectroglottogram Electroglottogram_highPassFilter (Electroglottogram me, double fromFrequency, double smoothing) {
	try {
		autoElectroglottogram thee = Data_copy (me);
		autoSpectrum spec = Sound_to_Spectrum (me, true);
		Spectrum_passHannBand (spec.get(), fromFrequency, spec -> xmax, smoothing);
		autoSound him = Spectrum_to_Sound (spec.get());
		thy z.row (1) <<= his z.row (1).part (1, thy nx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not high-pass filered.");
	}
}

autoSound Electroglottogram_to_Sound (Electroglottogram me) {
	try {
		autoSound thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
		thy z.row (1) <<= my z.row (1);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound.");
	}
}

/* End of file Electroglottogram.cpp */

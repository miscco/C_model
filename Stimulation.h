/*
 *	Copyright (c) 2014 University of Lübeck
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *	THE SOFTWARE.
 *
 *	AUTHORS:	Michael Schellenberger Costa: mschellenbergercosta@gmail.com
 *
 *	Based on:	Characterization of K-Complexes and Slow Wave Activity in a Neural Mass Model
 *				A Weigenand, M Schellenberger Costa, H-VV Ngo, JC Claussen, T Martinetz
 *				PLoS Computational Biology. 2014;10:e1003923
 */

/******************************************************************************/
/*					Implementation of the stimulation protocol				  */
/******************************************************************************/
#pragma once
#include <vector>

#include "Cortical_Column.h"
#include "Random_Stream.h"

/******************************************************************************/
/*								Stimulation object							  */
/******************************************************************************/
class Stim {
public:
    /* Empty constructor for compiling */
    Stim(void);

    Stim(Cortical_Column& C, double* var)
    { Cortex = &C; setup(var);}

    /* Initialize stimulation class with respect to stimulation mode */
    void setup (double* var_stim);

    /* Check whether stimulation should be started/stopped */
    void check_stim	(int time);
private:
    /* Mode of stimulation 	*/
    /* 0 == none 			*/
    /* 1 == semi-periodic	*/
    /* 2 == phase dependent */
    int mode			= 0;

    /* Default values already in dt: E1==ms,  E4==s	*/
    /* Stimulation strength 						*/
    double 	strength 				= 0.0;

    /* Duration of the stimulation */
    int 	duration 				= 120E1;

    /* Interval between different Stimulus events */
    int 	ISI						= 5E4;

    /* Range of Inter Stimulus Interval */
    int 	ISI_range 				= 1E4;

    /* Number of stimuli in case of multiple stimuli per event */
    int		number_of_stimuli 		= 1;

    /* Time until next stimulus */
    /* Function varies between different stimulation modes */
    int		time_to_stimuli			= 350E1;

    /* Time between stimuli in case of multiple stimuli per event */
    int 	time_between_stimuli 	= 1050E1;

    /* Threshold for phase dependent stimulation */
    double 	threshold				= -72;

    /* Internal variables */
    /* Simulation on for TRUE and off for FALSE */
    bool 	stimulation_started 	= false;

    /* If threshold has been reached */
    bool 	threshold_crossed		= false;

    /* If minimum was found */
    bool 	minimum_found			= false;

    /* If a stimulation event has occurred and there is a minimal time (pause) until the next one */
    bool 	stimulation_paused 		= false;

    /* Onset in time steps where no data is recorded, so stimulation has to be delayed */
    int 	onset_correction		= 10E4;

    /* Counter for number of stimuli that occurred within a stimulation event */
    int 	count_stimuli 			= 1;

    /* Counter for stimulation duration since started*/
    int 	count_duration			= 0;

    /* Counter after minimum was found */
    int 	count_to_start 			= 0;

    /* Counter for time between two stimulation events (with multiple tones) */
    int 	count_pause 			= 0;

    /* Old voltage value for minimum detection */
    double 	Vp_old					= 0;

    /* Pointer to columns */
    Cortical_Column* Cortex;

    /* Data containers */
    std::vector<int> marker_stimulation;

    /* Random number generator in case of semi-periodic stimulation */
    randomStreamUniformInt Uniform_Distribution = randomStreamUniformInt(0, 0);

    /* Create MATLAB container for marker storage */
    friend mxArray* get_marker(Stim &stim);
};

/******************************************************************************/
/*							Function definitions							  */
/******************************************************************************/
void Stim::setup (double* var_stim) {
    extern const int onset;
    extern const int res;

    /* Set the onset onset_correction for the marker */
    onset_correction 		= onset * res;

    /* Mode of stimulation */
    mode					= (int) var_stim[0];

    /* Scale the stimulation strength from s^-1 (Hz) to ms^-1 */
    strength 				= 		var_stim[1] / 1000;

    /* Scale duration from ms to dt */
    duration 				= (int) var_stim[2] * res / 1000;

    /* Scale the inter stimulus event interval from s to dt */
    ISI 					= (int) var_stim[3] * res;

    /* Scale inter stimulus event interval range from s to dt */
    ISI_range 				= (int) var_stim[4] * res;

    /* Number of stimuli per Stimulus event */
    number_of_stimuli		= (int) var_stim[5];

    /* Scale time_between_stimuli from ms to dt */
    time_between_stimuli 	= (int) var_stim[6] * res / 1000;

    /* If ISI is fixed do not create RNG */
    if (mode == 1) {
        /* Set first time_to_stimuli to 1 sec after onset */
        time_to_stimuli = (int) (onset+1) * res;

        /* If ISI is random create RNG */
        if (ISI_range != 0){
            /* Generate uniform distribution */
            Uniform_Distribution = randomStreamUniformInt(ISI-ISI_range, ISI+ISI_range);
        }
    } else {
        /* In case of phase dependent stimulation, time_to_stim is the time from
         * minimum detection to start of stimulation. Scale time_to_stimuli from
         * ms to dt */
        time_to_stimuli = (int) var_stim[7] * res / 1000;
    }
}

void Stim::check_stim	(int time) {
    /* Check if stimulation should start */
    switch (mode) {

    /* No stimulation */
    default:
        break;

    /* Semi-periodic stimulation */
    case 1:
        /* Check if stimulation time is reached */
        if(time == time_to_stimuli) {
            /* Switch stimulation on */
            stimulation_started 	= true;
            Cortex->set_input(strength);

            /* Add marker for the first stimuli in the event */
            if(count_stimuli == 1) {
                marker_stimulation.push_back(time - onset_correction);
            }

            /* Check if multiple stimuli should be applied */
            if (count_stimuli < number_of_stimuli) {
                /* Update the timer with respect to time between stimuli */
                time_to_stimuli += time_between_stimuli;
                count_stimuli++;
            }
            /* After last stimulus in event update the timer with respect to (random) ISI*/
            else {
                time_to_stimuli += (ISI_range==0)? ISI : Uniform_Distribution();

                /* Reset the stimulus counter for next stimulation event */
                count_stimuli = 1;
            }
        }
        break;

    /* Phase dependent stimulation */
    case 2:
        /* Search for threshold */
        if(!stimulation_started &&
           !minimum_found       &&
           !threshold_crossed   &&
           !stimulation_paused  &&
           time>onset_correction) {
            if(Cortex->Vp[0]<=threshold) {
                threshold_crossed 	= true;
            }
        }

        /* Search for minimum */
        if(threshold_crossed) {
            if(Cortex->Vp[0]>Vp_old) {
                threshold_crossed 	= false;
                minimum_found 		= true;
                Vp_old = 0;
            } else {
                Vp_old = Cortex->Vp[0];
            }
        }

        /* Wait until the stimulation should start */
        if(minimum_found) {
            /* Start stimulation after time_to_stimuli has passed */
            if(count_to_start==time_to_stimuli + (count_stimuli-1) * time_between_stimuli) {
                stimulation_started 	= true;
                Cortex->set_input(strength);

                /* Add marker for the first stimuli in the event */
                if(count_stimuli == 1) {
                    marker_stimulation.push_back(time - onset_correction);
                }

                /* Check if multiple stimuli should be applied */
                if (count_stimuli < number_of_stimuli) {
                    /* Update the number of stimuli */
                    count_stimuli++;
                } else {
                    /* After last stimulus in event pause the stimulation */
                    minimum_found 			= false;
                    stimulation_paused 		= true;
                    count_to_start 			= 0;

                    /* Reset the stimulus counter for next stimulation event */
                    count_stimuli = 1;
                }
            }
            count_to_start++;
        }
        break;
    }

    /* Wait to switch the stimulation off */
    if(stimulation_started) {
        if(count_duration==duration) {
            stimulation_started 	= false;
            count_duration			= 0;
            Cortex->set_input(0.0);
        }
        count_duration++;
    }

    /* Wait if there is a pause between stimulation events */
    if(stimulation_paused) {
        if(count_pause == ISI) {
            stimulation_paused	= false;
            count_pause 		= 0;
        }
        count_pause++;
    }
}

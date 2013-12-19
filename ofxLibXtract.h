/*
 *  ofxLibXtract.h
 *  libXtractAudioInput
 *
 *  Created by Andrew on 26/11/2013.
 *  Copyright 2013 QMUL. All rights reserved.
 *
 */


#ifndef OFX_LIBXTRACT
#define OFX_LIBXTRACT

#include "libxtract.h"
#include <vector>
#include <deque>

#define BLOCKSIZE 1024 /* Jamie Bullock's FIX: this should be dynamic - somehow */
#define NYQUIST 22050.0f

#define HALF_BLOCKSIZE BLOCKSIZE >> 1
#define SAMPLERATE 44100
#define PERIOD 102
#define MFCC_FREQ_BANDS 13
#define MFCC_FREQ_MIN 20
#define MFCC_FREQ_MAX 20000

/*
typedef enum waveform_type_
{
    SINE,
    SAWTOOTH,
    SQUARE,
    NOISE
} 
waveform_type;
 */

class ofxLibXtract{
public:
	
	//want to be able to compute features with libxtract
	//so set up some things like MFCC filters
	//and hand it a block of audio to be processed
	
	ofxLibXtract();
	~ofxLibXtract();
	
	//void fill_wavetable(const float frequency, waveform_type type);
	
	void copyAudioToBuffer(float* audioIn, int chunkSize);
	void calculateFeatures(float* audioIn, int chunkSize);
	void makeWindowedAudio();
	void calculateSpectrum();
	void calculateMFCCs();
	int simpleTest();
	
	
	
	double windowed[BLOCKSIZE];// = {0};
	
	double audioArray[BLOCKSIZE];
	double spectrum[BLOCKSIZE];
	std::vector<double> mfccs;//[MFCC_FREQ_BANDS];
	std::deque<double> bufferedAudio;

	double *window;
	double centroid;
	
	xtract_mel_filter mel_filters;
	
	double argd[4];
};
#endif
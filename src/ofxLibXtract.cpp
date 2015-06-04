/*
 *  ofxLibXtract.cpp
 *  libXtractAudioInput
 *
 *  Created by Andrew on 26/11/2013.
 *  Copyright 2013 QMUL. All rights reserved.
 *
 */

#include "ofxLibXtract.h"


//#include "xtract/libxtract.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

const bool printingOn = true;//true;//false;

double wavetable[BLOCKSIZE];
typedef enum waveform_type_
{
    SINE,
    SAWTOOTH,
    SQUARE,
    NOISE
} 
waveform_type;


void fill_wavetable(const float frequency, waveform_type type)
{
	
	int samples_per_period = SAMPLERATE / frequency;
	
	for (int i = 0; i < BLOCKSIZE; ++i)
	{
		int phase = i % samples_per_period;
		
		switch (type)
		{
			case SINE:
				wavetable[i] = sin((phase / (double)samples_per_period)*2*M_PI);//(double)PERIOD) * 2 * M_PI);
				break;
			case SQUARE:
				if (phase < (samples_per_period / 2.f))     
				{
					wavetable[i] = -1.0;
				}
				else
				{
					wavetable[i] = 1.0;
				}
				break;
			case SAWTOOTH:
				wavetable[i] = ((phase / (double)PERIOD) * 2) - 1.;
				break;
			case NOISE:
				wavetable[i] = 0.05675+((rand()%1000)/500.0)-1.0;//(arc4random_uniform(1000) / 500.0) - 1;
				break; 
		}
	}
}

void print_wavetable(void)
{
	for (int i = 0; i < BLOCKSIZE; ++i)
	{
		printf("%f\n", wavetable[i]);
	}
}



ofxLibXtract::ofxLibXtract(){
	
	fill_wavetable(256.0, SINE);
	print_wavetable();
	double mean = 1;
	xtract[XTRACT_MEAN](wavetable, BLOCKSIZE, NULL, &mean);
	printf("MEAN %f\n", mean);
	

	
	
	
	int tmp = simpleTest();
	printf("Simpletest returned %i\n", tmp);
	
	for (int i = 0; i < BLOCKSIZE; i++)
	{
		windowed[i] = 0;
		spectrum[i] = 0;
		peaks[i] = 0;
	}
	mfccs.assign(MFCC_FREQ_BANDS, 0.0);
	bufferedAudio.assign(BLOCKSIZE, 0.0);
	window = NULL;
	window = xtract_init_window(BLOCKSIZE, XTRACT_HANN);
	//argd = {0.0, 0.0, 0.0, 0.0};
	
	xtract_init_fft(BLOCKSIZE, XTRACT_SPECTRUM);
	
	bufferSamplesCopied = 0;
	
	
	//testing spec centroid of sine wave above
	// get the spectrum 
    argd[0] = SAMPLERATE / (double)BLOCKSIZE;
    argd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argd[2] = 0.f; // DC component - we expect this to zero for square wave 
    argd[3] = 0.f; // No Normalisation 
	
	xtract_windowed(wavetable, BLOCKSIZE, window, windowed);
	//    xtract_init_fft(BLOCKSIZE, XTRACT_SPECTRUM);
    xtract[XTRACT_SPECTRUM](windowed, BLOCKSIZE, &argd[0], spectrum);
	//was free fft
	
	double centroid = -1;
	xtract[XTRACT_SPECTRAL_CENTROID](spectrum, BLOCKSIZE, NULL, &centroid);
    if (printingOn)
		printf("\nSpectral_Centroid: %f\n", centroid);
	//end test
	
	
	//xtract_init_fft(BLOCKSIZE, XTRACT_SPECTRUM);
}

ofxLibXtract::~ofxLibXtract(){
    xtract_free_window(window);
	xtract_free_fft();
	//xtract_free_fft();
}

bool ofxLibXtract::copyAudioToBuffer(float* audioIn, int chunkSize){
	/*
	printf("libxtract audio %i\n", chunkSize);
	for (int i = 0;i   < 12; i++)
		printf("%.4f\n", audioIn[i]);
	*/
	
	bool newBuffer = false;
	
	for (int i = 0; i < BLOCKSIZE - chunkSize; i++)
		audioArray[i] = audioArray[i+chunkSize];

	int tmp = BLOCKSIZE-chunkSize;
	
	for (int i = 0; i < chunkSize; i++)
		audioArray[i+tmp] = audioIn[i];
	
	bufferSamplesCopied += chunkSize;
	
	if (bufferSamplesCopied == HOPSIZE){
		newBuffer = true;
		bufferSamplesCopied = 0;
	}
		 
	/*
	for (int i = 0; i < chunkSize; i++){
		//printf("input[%i] %.4f\n", i, audioIn[i]);
		bufferedAudio.pop_front();
		bufferedAudio.push_back(audioIn[i]);

	}
	for (int i = 0; i < BLOCKSIZE; i++)
		audioArray[i] = bufferedAudio[i];
	 */
	
	//returns true when we need to process as we have copied another HOPSIZE samples across
	return newBuffer;
}

void ofxLibXtract::calculateFeatures(float* audioIn, int chunkSize){
	if (copyAudioToBuffer(audioIn, chunkSize)){
		makeWindowedAudio();
		calculateSpectrum();
		calculateMFCCs();
	}
}

void ofxLibXtract::makeWindowedAudio(){
	
	// create the window function
	//double *window = NULL;
	
    xtract_windowed(audioArray, BLOCKSIZE, window, windowed);
	
//	for (int i = 0; i < BLOCKSIZE; i++)
//		printf("buffered audio [%i] %f WINDOWED %f\n", i, bufferedAudio[i], windowed[i]);
}

void ofxLibXtract::calculateSpectrum(){
    // get the spectrum 
    argd[0] = SAMPLERATE / (double)BLOCKSIZE;
    argd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argd[2] = 0.f; // DC component - we expect this to zero for square wave 
    argd[3] = 0.f; // No Normalisation 
	
//    xtract_init_fft(BLOCKSIZE, XTRACT_SPECTRUM);
    xtract[XTRACT_SPECTRUM](windowed, BLOCKSIZE, &argd[0], spectrum);
	//was free fft
	
    xtract[XTRACT_SPECTRAL_CENTROID](spectrum, BLOCKSIZE, NULL, &centroid);
    printf("Spectral Centroid: %f\n", centroid);
	
/*	if (centroid > 22000){
		for (int i = 0; i < BLOCKSIZE; i++)
			printf("buffered audio [%i] %f WINDOWED %f\n", i, bufferedAudio[i], windowed[i]);
	}
 */
	
//	for (int i = 0; i < BLOCKSIZE; i++)
//		printf("spectrum[%i] %f\n", i, spectrum[i]);
	
	
	/*
	 argd[0] = SAMPLERATE / (double)BLOCKSIZE;
	 
	 argd[2] = 0.f; // DC component - we expect this to zero for square wave 
    argd[3] = 0.f;
	
	argd[1] = 10.0; // peak threshold as %  of maximum peak 
    xtract[XTRACT_PEAK_SPECTRUM](spectrum, BLOCKSIZE / 2, argd, peaks);
	*/
	
	
//	for (int i = 0; i < BLOCKSIZE; i++){
//		printf("Peaks[%i] %f\n", i, peaks[i]);
//		if (peaks[i] > 0)			
//	}
	
}

void ofxLibXtract::calculateMFCCs(){
	// compute the MFCCs 
    int n;
	double mfccs[MFCC_FREQ_BANDS] = {0};
	
	mel_filters.n_filters = MFCC_FREQ_BANDS;
    mel_filters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        mel_filters.filters[n] = (double *)malloc(BLOCKSIZE * sizeof(double));
    }
	
    xtract_init_mfcc(BLOCKSIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mel_filters.n_filters, mel_filters.filters);
    xtract_mfcc(spectrum, BLOCKSIZE >> 1, &mel_filters, mfccs);
	
    // print the MFCCs 
	if (printingOn){
		printf("MFCCs:\n");
		for(n = 0; n < MFCC_FREQ_BANDS; ++n)
		{
			printf("band: %d\t", n);
			if(n < 10) {
				printf("\t");
			}
			printf("coeff: %f\n", mfccs[n]);
		}
	}
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        this->mfccs[n] = mfccs[n];
    }

}


int ofxLibXtract::simpleTest(){
	printf("Libxtract simpletest\n");
	//somethign with the wavetable (eg NOISE defined elsewhere throwing this)
	/*
	 double mean = 0.0; 
    double f0 = 0.0;
    double flux = 0.0;
    double centroid = 0.0;
    double spectrum[BLOCKSIZE] = {0};
    double windowed[BLOCKSIZE] = {0};
    double peaks[BLOCKSIZE] = {0};
    double harmonics[BLOCKSIZE] = {0};
    double subframes[BLOCKSIZE] = {0};
    double difference[HALF_BLOCKSIZE] = {0};
    double *window = NULL;
    double mfccs[MFCC_FREQ_BANDS] = {0};
    double argd[4] = {0};
    double samplerate = 44100.0;
    int n;
    xtract_mel_filter mel_filters;
	
    fill_wavetable(344.53125f, NOISE); // 344.53125f = 128 samples @ 44100 Hz
    print_wavetable();
	
    // get the F0 
    xtract[XTRACT_WAVELET_F0](wavetable, BLOCKSIZE, &samplerate, &f0);
    printf("\nF0: %f\n", f0);
	
    // get the mean of the input 
    xtract[XTRACT_MEAN](wavetable, BLOCKSIZE, NULL, &mean);
    printf("\nInput mean = %.2f\n\n", mean); // We expect this to be zero for a square wave   
	
    // create the window function 
    window = xtract_init_window(BLOCKSIZE, XTRACT_HANN);
    xtract_windowed(wavetable, BLOCKSIZE, window, windowed);
    xtract_free_window(window);
	
    // get the spectrum 
    argd[0] = SAMPLERATE / (double)BLOCKSIZE;
    argd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argd[2] = 0.f; // DC component - we expect this to zero for square wave 
    argd[3] = 0.f; // No Normalisation 
	
    xtract_init_fft(BLOCKSIZE, XTRACT_SPECTRUM);
    xtract[XTRACT_SPECTRUM](windowed, BLOCKSIZE, &argd[0], spectrum);
    xtract_free_fft();
	
    xtract[XTRACT_SPECTRAL_CENTROID](spectrum, BLOCKSIZE, NULL, &centroid);
    printf("\nSpectral Centroid: %f\n", centroid);
	
    argd[1] = 10.0; // peak threshold as %  of maximum peak 
    xtract[XTRACT_PEAK_SPECTRUM](spectrum, BLOCKSIZE / 2, argd, peaks);
	
    argd[0] = f0;
    argd[1] = .3; // harmonic threshold 
    xtract[XTRACT_HARMONIC_SPECTRUM](peaks, BLOCKSIZE, argd, harmonics);
	
    // print the spectral bins 
    printf("\nSpectrum:\n");
    for(n = 0; n < (BLOCKSIZE >> 1); ++n)
    {
        printf("freq: %.1f\tamp: %.6f", spectrum[n + (BLOCKSIZE >> 1)], spectrum[n]);
        if (peaks[n + (BLOCKSIZE >> 1)] != 0.f)
        {
            printf("\tpeak:: freq: %.1f\tamp: %.6f\n", peaks[n + (BLOCKSIZE >> 1)], peaks[n]);
        }
        else
        {
            printf("\n");
        }
    }
    printf("\n");
	
    // compute the MFCCs 
    mel_filters.n_filters = MFCC_FREQ_BANDS;
    mel_filters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        mel_filters.filters[n] = (double *)malloc(BLOCKSIZE * sizeof(double));
    }
	
    xtract_init_mfcc(BLOCKSIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mel_filters.n_filters, mel_filters.filters);
    xtract_mfcc(spectrum, BLOCKSIZE >> 1, &mel_filters, mfccs);
	
    // print the MFCCs 
    printf("MFCCs:\n");
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        printf("band: %d\t", n);
        if(n < 10) {
            printf("\t");
        }
        printf("coeff: %f\n", mfccs[n]);
    }
	
    // compute Spectral Flux 
    argd[0] = SAMPLERATE / HALF_BLOCKSIZE;
    argd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argd[2] = 0.f; // DC component 
    argd[3] = 0.f; // No Normalisation 
    
    xtract_init_fft(HALF_BLOCKSIZE, XTRACT_SPECTRUM);
    xtract_features_from_subframes(wavetable, BLOCKSIZE, XTRACT_SPECTRUM, argd, subframes);
    xtract_difference_vector(subframes, BLOCKSIZE, NULL, difference);
    
    argd[0] = 1.0; // norm order 
    argd[1] = XTRACT_POSITIVE_SLOPE; // positive slope 
    
    xtract_flux(difference, HALF_BLOCKSIZE, argd, &flux);
    
    printf("Flux: %f\n", flux);
	
    // cleanup 
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        free(mel_filters.filters[n]);
    }
    free(mel_filters.filters);
	*/
    return 0;
	
	
}
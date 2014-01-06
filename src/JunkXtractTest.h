/*
 *  JunkXtractTest.h
 *  libXtractAudioInput
 *
 *  Created by Andrew on 05/01/2014.
 *  Copyright 2014 QMUL. All rights reserved.
 *
 */

/*
 
 
 //#define BLOCKSIZE 512//1024 /* Jamie Bullock's FIX: this should be dynamic - somehow */
//#define NYQUIST 22050.0f

//#define HALF_BLOCKSIZE BLOCKSIZE >> 1
//#define SAMPLERATE 44100
//#define PERIOD 102
//#define MFCC_FREQ_BANDS 13
//#define MFCC_FREQ_MIN 20
//#define MFCC_FREQ_MAX 20000

/*
void xtractTest(){
	
	
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
	
	fill_wavetable(344.53125f, NOISE);
	print_wavetable();

	
	
	xtract[XTRACT_WAVELET_F0](wavetable, BLOCKSIZE, &samplerate, &f0);
    printf("\nF0: %f\n", f0);
	
	
    // get the mean of the input 
    xtract[XTRACT_MEAN](wavetable, BLOCKSIZE, NULL, &mean);
    printf("\nInput mean = %.4f\n\n", mean); // We expect this to be zero for a square wave ]/  
	
	
    // create the window function ]/
    window = xtract_init_window(BLOCKSIZE, XTRACT_HANN);
    xtract_windowed(wavetable, BLOCKSIZE, window, windowed);
    xtract_free_window(window);
	
    // get the spectrum ]/
    argd[0] = SAMPLERATE / (double)BLOCKSIZE;
    argd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argd[2] = 0.f; // DC component - we expect this to zero for square wave ]/
    argd[3] = 0.f; // No Normalisation ]/
	
    xtract_init_fft(BLOCKSIZE, XTRACT_SPECTRUM);
    xtract[XTRACT_SPECTRUM](windowed, BLOCKSIZE, &argd[0], spectrum);
    xtract_free_fft();
	
    xtract[XTRACT_SPECTRAL_CENTROID](spectrum, BLOCKSIZE, NULL, &centroid);
    printf("\nSpectral Centroid: %f\n", centroid);
	
	argd[1] = 10.0; // peak threshold as %  of maximum peak ]/
    xtract[XTRACT_PEAK_SPECTRUM](spectrum, BLOCKSIZE / 2, argd, peaks);
	
    argd[0] = f0;
    argd[1] = .3; // harmonic threshold ]/
    xtract[XTRACT_HARMONIC_SPECTRUM](peaks, BLOCKSIZE, argd, harmonics);
	
    // print the spectral bins ]/
	// printf("\nSpectrum:\n");
    //
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
	 ]/
	
	// compute the MFCCs ]/
    mel_filters.n_filters = MFCC_FREQ_BANDS;
    mel_filters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        mel_filters.filters[n] = (double *)malloc(BLOCKSIZE * sizeof(double));
    }
	
    xtract_init_mfcc(BLOCKSIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mel_filters.n_filters, mel_filters.filters);
    xtract_mfcc(spectrum, BLOCKSIZE >> 1, &mel_filters, mfccs);
	
    // print the MFCCs ]/
    printf("MFCCs:\n");
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        printf("band: %d\t", n);
        if(n < 10) {
            printf("\t");
        }
        printf("coeff: %f\n", mfccs[n]);
    }
	
	
    // compute Spectral Flux ]/
    argd[0] = SAMPLERATE / HALF_BLOCKSIZE;
    argd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argd[2] = 0.f; // DC component ]/
    argd[3] = 0.f; // No Normalisation ]/
    
    xtract_init_fft(HALF_BLOCKSIZE, XTRACT_SPECTRUM);
    xtract_features_from_subframes(wavetable, BLOCKSIZE, XTRACT_SPECTRUM, argd, subframes);
    xtract_difference_vector(subframes, BLOCKSIZE, NULL, difference);
    
    argd[0] = 1.0; // norm order ]/
    argd[1] = XTRACT_POSITIVE_SLOPE; // positive slope ]/
    
    xtract_flux(difference, HALF_BLOCKSIZE, argd, &flux);
    
    printf("Flux: %f\n", flux);
	
    // cleanup ]/
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        free(mel_filters.filters[n]);
    }
    free(mel_filters.filters);
	
	
}

*/


/*
 float mean = 0;
 float myvector[] = {.1, .2, .3, .4, -.5, -.4, -.3, -.2, -.1};
 float spectrum[10];
 int n, N = 9;
 float argf[4];
 
 argf[0] = 8000.f;
 argf[1] = XTRACT_MAGNITUDE_SPECTRUM;
 argf[2] = 0.f;
 argf[3] = 0.f;
 double f0 = 0.0;
 double samplerate = 44100.0;
 */

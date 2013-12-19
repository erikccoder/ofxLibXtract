#include "testApp.h"

#include <math.h>
#include <stdlib.h>
#include <cstdlib>

#include "assert.h"
//TO GET THIS WORKING
//clone LibXtract using "git clone <libxtract git repo address>"
//add the xtract and src directories here (drag them into project)
//compile
//I didn't need to compile LibXtract. 
//That would create a library on the computer but it seems fine this way
//add in the header inlcude a recursiove path to "/Users/yourpath/libxtract"

double wavetable[BLOCKSIZE];
typedef enum waveform_type_
{
    SINE,
    SAWTOOTH,
    SQUARE,
    NOISE
} 
waveform_type;

//--------------------------------------------------------------
void testApp::setup(){	 
	
	xtractTest();
	
	xtractFeature = 1;
	
	if(xtractFeature == XTRACT_PEAK_SPECTRUM)
		printf("one");
	
	ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	ofBackground(54, 54, 54);	
	
	// 0 output channels, 
	
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	
	soundStream.listDevices();
	
	//if you want to set a different device id 
	soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
	
	int bufferSize = 256;
	
	
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);
	
	bufferedAudio.assign(BLOCKSIZE, 0.0);//to hold our real-time audio
	
	bufferCounter	= 0;
	drawCounter		= 0;
	smoothedVol     = 0.0;
	scaledVol		= 0.0;

	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);

}

//--------------------------------------------------------------
void testApp::update(){
	//lets scale the vol up to a 0-1 range 
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	//lets record the volume into an array
	volHistory.push_back( scaledVol );
	
	//if we are bigger the the size we want to record - lets drop the oldest value
	if( volHistory.size() >= 400 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofSetColor(225);
	ofDrawBitmapString("AUDIO INPUT EXAMPLE", 32, 32);
	ofDrawBitmapString("press 's' to unpause the audio\n'e' to pause the audio", 31, 92);
	
	ofNoFill();
	
	// draw the left channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 170, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Left Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofRect(0, 0, 512, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (int i = 0; i < left.size(); i++){
				ofVertex(i*2, 100 -left[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();

	// draw the right channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 470, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Right Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofRect(0, 0, 512, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (int i = 0; i < right.size(); i++){
				ofVertex(i*2, 100 -right[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
	
	// draw the average volume:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(565, 170, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(scaledVol * 100.0, 0), 4, 18);
		ofRect(0, 0, 400, 400);
		
		ofSetColor(245, 58, 135);
		ofFill();		
		ofCircle(200, 200, scaledVol * 190.0f);
		
		//lets draw the volume history as a graph
		ofBeginShape();
		for (int i = 0; i < volHistory.size(); i++){
			if( i == 0 ) ofVertex(i, 400);

			ofVertex(i, 400 - volHistory[i] * 70);
			
			if( i == volHistory.size() -1 ) ofVertex(i, 400);
		}
		ofEndShape(false);		
			
		ofPopMatrix();
	ofPopStyle();
	
	drawCounter++;
	
	ofSetColor(225);
	string reportString = "buffers received: "+ofToString(bufferCounter)+"\ndraw routines called: "+ofToString(drawCounter)+"\nticks: " + ofToString(soundStream.getTickCount());
	ofDrawBitmapString(reportString, 32, 589);
	
	int height = ((22050.0-ofxlibx.centroid)*ofGetHeight())/22050.0f;
	ofSetColor(ofColor::bisque);
	ofCircle(500, height, 10);
	
	ofDrawBitmapString("specCentroid "+ofToString(ofxlibx.centroid, 0), 20, 400);
		
}

//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){	
	
	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;	
	
	assert(nChannels == 2);
	
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	for (int i = 0; i < bufferSize; i++){
	//	assert (input[i] < 1.001);
	//	assert (input[i] > -1.001);
		
		left[i]		= input[i*2]*0.5;
		
	//	assert (left[i] < 1.001);
	//	assert (left[i] > -1.001);
		
		
		right[i]	= input[i*2+1]*0.5;

		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
		
	//	bufferedAudio.pop_front();
	//	bufferedAudio.push_back(left[i]);//+right[i]);
	}
	
	//assert(bufferedAudio.size() == BLOCKSIZE);
	
	//this is how we get the mean of rms :) 
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :) 
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
	
	bufferCounter++;
	
//	ofxlibx.calculateMFCCs(&bufferedAudio[0], BLOCKSIZE);
	ofxlibx.calculateFeatures(&left[0], bufferSize);
	
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void fill_wavetable(const float frequency, waveform_type type)
{
	
    int samples_per_period = SAMPLERATE / frequency;
	
    for (int i = 0; i < BLOCKSIZE; ++i)
    {
        int phase = i % samples_per_period;
		
        switch (type)
        {
            case SINE:
                wavetable[i] = sin((phase / (double)PERIOD) * 2 * M_PI);
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

void testApp::xtractTest(){
	
	
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
	
	
	xtract[XTRACT_WAVELET_F0](wavetable, BLOCKSIZE, &samplerate, &f0);
    printf("\nF0: %f\n", f0);
	
	
    /* get the mean of the input */
    xtract[XTRACT_MEAN](wavetable, BLOCKSIZE, NULL, &mean);
    printf("\nInput mean = %.4f\n\n", mean); /* We expect this to be zero for a square wave */  
	
	
    /* create the window function */
    window = xtract_init_window(BLOCKSIZE, XTRACT_HANN);
    xtract_windowed(wavetable, BLOCKSIZE, window, windowed);
    xtract_free_window(window);
	
    /* get the spectrum */
    argd[0] = SAMPLERATE / (double)BLOCKSIZE;
    argd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argd[2] = 0.f; /* DC component - we expect this to zero for square wave */
    argd[3] = 0.f; /* No Normalisation */
	
    xtract_init_fft(BLOCKSIZE, XTRACT_SPECTRUM);
    xtract[XTRACT_SPECTRUM](windowed, BLOCKSIZE, &argd[0], spectrum);
    xtract_free_fft();
	
    xtract[XTRACT_SPECTRAL_CENTROID](spectrum, BLOCKSIZE, NULL, &centroid);
    printf("\nSpectral Centroid: %f\n", centroid);
	
	argd[1] = 10.0; /* peak threshold as %  of maximum peak */
    xtract[XTRACT_PEAK_SPECTRUM](spectrum, BLOCKSIZE / 2, argd, peaks);
	
    argd[0] = f0;
    argd[1] = .3; /* harmonic threshold */
    xtract[XTRACT_HARMONIC_SPECTRUM](peaks, BLOCKSIZE, argd, harmonics);
	
    /* print the spectral bins */
   // printf("\nSpectrum:\n");
    /*
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
	*/
	
	/* compute the MFCCs */
    mel_filters.n_filters = MFCC_FREQ_BANDS;
    mel_filters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        mel_filters.filters[n] = (double *)malloc(BLOCKSIZE * sizeof(double));
    }
	
    xtract_init_mfcc(BLOCKSIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mel_filters.n_filters, mel_filters.filters);
    xtract_mfcc(spectrum, BLOCKSIZE >> 1, &mel_filters, mfccs);
	
    /* print the MFCCs */
    printf("MFCCs:\n");
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        printf("band: %d\t", n);
        if(n < 10) {
            printf("\t");
        }
        printf("coeff: %f\n", mfccs[n]);
    }
	
	
    /* compute Spectral Flux */
    argd[0] = SAMPLERATE / HALF_BLOCKSIZE;
    argd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argd[2] = 0.f; /* DC component */
    argd[3] = 0.f; /* No Normalisation */
    
    xtract_init_fft(HALF_BLOCKSIZE, XTRACT_SPECTRUM);
    xtract_features_from_subframes(wavetable, BLOCKSIZE, XTRACT_SPECTRUM, argd, subframes);
    xtract_difference_vector(subframes, BLOCKSIZE, NULL, difference);
    
    argd[0] = 1.0; /* norm order */
    argd[1] = XTRACT_POSITIVE_SLOPE; /* positive slope */
    
    xtract_flux(difference, HALF_BLOCKSIZE, argd, &flux);
    
    printf("Flux: %f\n", flux);
	
    /* cleanup */
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        free(mel_filters.filters[n]);
    }
    free(mel_filters.filters);
	
	
}

#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "libxtract.h"

#include "ofxLibXtract.h"


#define BLOCKSIZE 1024 /* Jamie Bullock's FIX: this should be dynamic - somehow */
#define NYQUIST 22050.0f

#define HALF_BLOCKSIZE BLOCKSIZE >> 1
#define SAMPLERATE 44100
#define PERIOD 102
#define MFCC_FREQ_BANDS 13
#define MFCC_FREQ_MIN 20
#define MFCC_FREQ_MAX 20000

class testApp : public ofBaseApp{
	
	public:
		
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
				
		void audioIn(float * input, int bufferSize, int nChannels); 
	
		vector <float> left;
		vector <float> right;
		vector <float> volHistory;
		
	
		std::deque<float> bufferedAudio;
	
		int 	bufferCounter;
		int 	drawCounter;
		
		float smoothedVol;
		float scaledVol;
		
		ofSoundStream soundStream;
	
		void xtractTest();
		int xtractFeature;
	
		xtract_mel_filter *mf;
	    xtract_function_descriptor_t *fd;

		ofxLibXtract ofxlibx;
};

#endif	


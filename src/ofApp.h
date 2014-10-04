#pragma once

#include "ofMain.h"

#include "libxtract.h"
#include "ofxLibXtract.h"


class ofApp : public ofBaseApp{

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
		
    //xtra fns
    
    void drawLibxtractInfo();
    
    
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
	
    //void xtractTest();
    //int xtractFeature;
	
    //xtract_mel_filter *mf;
    //xtract_function_descriptor_t *fd;
    
    ofxLibXtract ofxlibx;
};

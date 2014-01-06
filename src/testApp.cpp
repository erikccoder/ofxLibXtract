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
/*

 
*/
//--------------------------------------------------------------
void testApp::setup(){	 
	
	//junkxtractTest();
	
	//xtractFeature = 1;
	
	//if(xtractFeature == XTRACT_PEAK_SPECTRUM)
	//	printf("extract peak - one");
	
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
	
	//bufferedAudio.assign(BLOCKSIZE, 0.0);//to hold our real-time audio
	
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
	
	//oldDraw();
	drawLibxtractInfo();
	
	
	/*
	double maxVal = 11025.0;//22050.0;//of spec centroid we expect
	int height = ((maxVal-ofxlibx.centroid)*ofGetHeight())/maxVal;
	ofSetColor(ofColor::bisque);
	ofFill();
	ofCircle(500, height, 10);
	
	*/
	
	
	
	/*
	for (int i = 0; i < BLOCKSIZE; i++){
		if (ofxlibx.peaks[i] > 0){
			ofSetColor(ofColor::gold);
			int y = ofGetHeight()*(1.0-(ofxlibx.peaks[i] / 11000.));
			ofLine(0, y, 1000, y);
		}
	}
	*/

}


void testApp::drawLibxtractInfo(){
	ofNoFill();
	ofDrawBitmapString("specCentroid "+ofToString(ofxlibx.centroid, 0), 20, 400);
	
	ofSetColor(ofColor::green);
	//	for (int i = 0; i < BLOCKSIZE/2; i++){
	//		ofLine(i, ofGetHeight(), i, ofGetHeight()- ofxlibx.spectrum[i]*10000.);
	//	}
	double factor = 2;
	ofBeginShape();
	for (int i = 0; i < BLOCKSIZE/2; i++){
		ofVertex(i*factor, ofGetHeight()- ofxlibx.spectrum[i]*10000.);
	}
	ofEndShape(false);
	ofSetLineWidth(2);
	ofSetColor(ofColor::red);
	int w = factor*((double)(ofxlibx.centroid*BLOCKSIZE)/SAMPLERATE);
	ofLine(w, ofGetHeight(), w, ofGetHeight()*0.5);
	ofSetLineWidth(1);

}



void testApp::oldDraw(){
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

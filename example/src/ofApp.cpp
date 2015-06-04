#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
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
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    drawLibxtractInfo();
}

void ofApp::drawLibxtractInfo()
{
    ofFill();
    
    stringstream ss;
    
    for (int i=0; i<ofxlibx.mfccs.size(); i++)
    {
//        ofCircle(ofxlibx.mfccs[i]*1000000.0, ofxlibx.mfccs[i+1]*1000000.0, 20);
        ofCircle(ofxlibx.mfccs[i]*1000.0, ofxlibx.mfccs[i+1]*1000.0, 20);
        ss << ofxlibx.mfccs[i] * 100000000 << endl;
    }
    ofDrawBitmapStringHighlight(ss.str(), 20,14);

	ofNoFill();
	ofDrawBitmapString("specCentroid "+ofToString(ofxlibx.centroid, 0), 20, 400);
	
	ofSetColor(ofColor::green);
	//	for (int i = 0; i < BLOCKSIZE/2; i++){
	//		ofLine(i, ofGetHeight(), i, ofGetHeight()- ofxlibx.spectrum[i]*10000.);
	//	}
	double factor = 2;
	ofBeginShape();
	for (int i = 0; i < BLOCKSIZE/2; i++){
		ofVertex(i*factor, ofGetHeight()- ofxlibx.spectrum[i]*100000.0);
	}

	ofEndShape(false);
	ofSetLineWidth(2);
	ofSetColor(ofColor::red);
	int w = factor*((double)(ofxlibx.centroid*BLOCKSIZE)/SAMPLERATE);
	ofLine(w, ofGetHeight(), w, ofGetHeight()*0.5);
	ofSetLineWidth(1);
    
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}




void ofApp::audioIn(float * input, int bufferSize, int nChannels){
	
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
	
//    	ofxlibx.calculateMFCCs(&bufferedAudio[0], BLOCKSIZE);
	ofxlibx.calculateFeatures(&left[0], bufferSize);

	
}

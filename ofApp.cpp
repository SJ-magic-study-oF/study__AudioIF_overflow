/************************************************************
************************************************************/
#include "ofApp.h"


/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(int _soundStream_Input_DeviceId, int _soundStream_Output_DeviceId)
: soundStream_Input_DeviceId(_soundStream_Input_DeviceId)
, soundStream_Output_DeviceId(_soundStream_Output_DeviceId)
, b_sleep_in(false)
, b_sleep_out(false)
, t_sleep_us(10000)
, t_sleepStep_us(1000)
, t_sleep_Min_us(1000)
{
}

/******************************
******************************/
ofApp::~ofApp()
{
}

/******************************
******************************/
void ofApp::exit()
{
	/********************
	ofAppとaudioが別threadなので、ここで止めておくのが安全.
	********************/
	soundStream.stop();
	soundStream.close();
	
	/********************
	********************/
	printf("\n\n");
}

//--------------------------------------------------------------
void ofApp::setup(){
	/********************
	********************/
	ofSetWindowTitle("Audio I/F");
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofSetWindowShape(WIDTH, HEIGHT);
	
	/********************
	********************/
	soundStream.listDevices();
	if( (soundStream_Input_DeviceId == -1) || (soundStream_Output_DeviceId == -1) ){
		ofExit();
		return;
	}
	// soundStream.setDeviceID(soundStream_DeviceId);
	/* set in & out respectively. */
	soundStream.setInDeviceID(soundStream_Input_DeviceId);  
	soundStream.setOutDeviceID(soundStream_Output_DeviceId);
	
	soundStream.setup(this, 2/* out */, 2/* in */, AUDIO_SAMPLERATE, AUDIO_BUF_SIZE, AUDIO_BUFFERS);
	
	samplesLeft.reserve(AUDIO_BUF_SIZE);  
    samplesRight.reserve(AUDIO_BUF_SIZE);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(20);
}

/******************************
audioIn/ audioOut
	同じthreadで動いている様子。
	また、audioInとaudioOutは、同時に呼ばれることはない(多分)。
	つまり、ofAppからaccessがない限り、変数にaccessする際にlock/unlock する必要はない。
	ofApp側からaccessする時は、threadを立てて、安全にpassする仕組みが必要(cf:NotUsed__thread_safeAccess.h)
	...ReferenceとSetで違う変数を用意し、このpassはthreadの向こう側で行う。
******************************/
void ofApp::audioIn(float *input, int bufferSize, int nChannels)
{
	/********************
	********************/
	t_in_AudioIn = ofGetElapsedTimef();
	
	/********************
	********************/
    for (int i = 0; i < bufferSize; i++) {
        samplesLeft[i] = input[2*i];  
        samplesRight[i] = input[2*i+1];  
    }
	
	/********************
	********************/
	if(b_sleep_in)	usleep(t_sleep_us);
	
	t_Duration_AudioIn = ofGetElapsedTimef() - t_in_AudioIn;
	
	// printf("(in:%10.4f, %10.4f), (out:%10.4f, %10.4f)\r", t_in_AudioIn, t_Duration_AudioIn, t_in_AudioOut, t_Duration_AudioOut);
	// fflush(stdout);
}  

/******************************
******************************/
void ofApp::audioOut(float *output, int bufferSize, int nChannels)
{
	/********************
	********************/
	t_in_AudioOut = ofGetElapsedTimef();
	
	/********************
	********************/
    for (int i = 0; i < bufferSize; i++) {  
        output[2*i] = samplesLeft[i];  
        output[2*i+1] = samplesRight[i];  
    }
	
	/********************
	********************/
	if(b_sleep_out)	usleep(t_sleep_us);
	
	t_Duration_AudioOut = ofGetElapsedTimef() - t_in_AudioOut;
	
	printf("(in:%10.4f, %10.4f), (out:%10.4f, %10.4f)\r", t_in_AudioIn, t_Duration_AudioIn, t_in_AudioOut, t_Duration_AudioOut);
	fflush(stdout);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case 'i':
			b_sleep_in = !b_sleep_in;
			printf("\nb_sleep_in = %d\n", b_sleep_in);
			break;
			
		case 'o':
			b_sleep_out = !b_sleep_out;
			printf("\nb_sleep_out = %d\n", b_sleep_out);
			break;
			
		case ' ':
			printf("\n");
			break;
			
		case OF_KEY_UP:
			t_sleep_us += t_sleepStep_us;
			break;
			
		case OF_KEY_DOWN:
			t_sleep_us -= t_sleepStep_us;
			if(t_sleep_us < t_sleep_Min_us) t_sleep_us = t_sleep_Min_us;
			break;
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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

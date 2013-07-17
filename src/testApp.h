#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "Clone.h"
#include "ofxFaceTracker.h"
#include "ofxFaceTrackerThreaded.h"
#include "trackerRemote.h"
#include "ofxXmlSettings.h"



class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void dragEvent(ofDragInfo dragInfo);
	void loadFace(string face);
//  void grabSrc();
    void grabFace();
	void keyPressed(int key);
    void remoteEvent(RemoteEvent &e);
    string rect2str(ofRectangle & rect);
    void drawDebug();
    void loadExt(string face);
    
    ofRectangle faceRect;
    ofImage mirror(ofPixels p);
    ofxXmlSettings prefs;
    ofxFaceTracker basicTracker;            //Tracker for analysis of captured images
	ofxFaceTrackerThreaded liveTracker;     //Tracker for live mapping
	ofVideoGrabber cam;
	ofImage extImage;
    ofImage camImage;
    ofImage trackedImage;
    ofImage maskImage;
    ofMesh camMesh;
    ofShader overlay;
    
	vector<ofVec2f> srcPoints;
	
    bool tracking = false;                   //Live Tracking is active
	bool cloneReady;                        //Cloner is initialized and running
    bool imgSnapped = true;                //A face image has been captured this session.
    bool cloning = true;                    //Cloning is active.
    bool scaleCV = false;                   //Down-res camera image for CV process.
    bool debug = false;
    bool tenEighty = false;

	Clone clone;
	ofFbo srcFbo, maskFbo, debugFbo;
    
	ofDirectory faces;
	int currentFace;
    int frameWidth = 640;
    int frameHeight = 480;
    int camWidth = 1280;
    int camHeight = 720;
    int drawWidth = 1280;
    int drawHeight = 720;
    int debugWidth = 240;
    int debugHeight = 427;
    int camRotation;
    string time;
    string tmp;
    string dbString;
    TrackerRemote remote;
};

#pragma once

#include "ofxFaceTracker.h"

class ofxFaceTrackerThreaded : public ofThread {
public:
    float _scale = 0.25;
    int _iterations = 5; // [1-25] 1 is fast and inaccurate, 25 is slow and accurate
    int _clamp = 2;  // [0-4] 1 gives a very loose fit, 4 gives a very tight fit
    float _tolerance = 0.5; // [.01-1] match tolerance
    float _attempts = 4;  // [1-4] 1 is fast and may not find faces, 4 is slow but will find faces

    
    

    
    
    void setup() {
		startThread(false, false);
       
	}
	void update(cv::Mat mat) {
		if(lock()) {
			ofxCv::copy(mat, buffer);
			unlock();
			newFrame = true;
			ofSleepMillis(30); // give the tracker a moment
		}
	}
	void draw() {
		if(lock()) {
			tracker->draw();
			unlock();
		}
	}
    
    void setScale(float s){
        if(lock()) {
            tracker->setRescale(s);
            unlock();
        }
    }
    
	bool getFound() {
		bool found = false;
		if(lock()) {
			found = tracker->getFound();
			unlock();
		}
		return found;
	}
	ofMesh getImageMesh() {
		ofMesh imageMesh;
		if(lock()) {
			imageMesh = tracker->getImageMesh();
			unlock();
		}
		return imageMesh;
	}
    
protected:
	void threadedFunction() {
		newFrame = false;
		tracker = new ofxFaceTracker();
        
        tracker->setRescale(_scale);
        tracker->setIterations(_iterations);
        tracker->setClamp(_clamp);
        tracker->setTolerance(_tolerance);
        tracker->setAttempts(_attempts);
        
        tracker->setup();
		while(isThreadRunning()) {
			if(newFrame) {
				if(lock()) {
					newFrame = false;
					tracker->update(buffer);
					unlock();
				}
			}
			ofSleepMillis(1);
		}
	}
	
		cv::Mat buffer;
	bool newFrame;
        ofxFaceTracker* tracker;
};
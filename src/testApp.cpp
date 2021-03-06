#include "testApp.h"

using namespace ofxCv;

void testApp::setup() {
#ifdef TARGET_OSX
//	ofSetDataPathRoot("../data/");
#endif
    ofSetDataPathRoot("../Resources/");

    //load settings
    prefs.loadFile("config.xml");
    int offX, offY;
    int full;
    int camID;
    int dbg;
    dbg =  prefs.getValue("DEBUG",99);
    offX = prefs.getValue("SCREEN:OFFX",0);
    offY = prefs.getValue("SCREEN:OFFY",0);
    full = prefs.getValue("SCREEN:FULLSCREEN",0);
    camID = prefs.getValue("CAM",0);
    camRotation = prefs.getValue("ROTATION",1);
    scaleCV = (prefs.getValue("CVSCALE",0) == 1) ? true : false;
    tenEighty = (prefs.getValue("FULLHD",0) == 1) ? true : false;
    drawWidth = (tenEighty) ? 1080 : 720;
    drawHeight = (tenEighty) ? 1920 : 1280;
    debug = (dbg) ? true : false;
    
    //setup window
    ofSetWindowPosition(offX,offY);
    if (full==1) ofToggleFullscreen();

    ofLog() << "Debugging: " << dbg << endl;

    //setup UDP biderectinal comm
    remote.setup();
    ofAddListener(RemoteEvent::events, this, &testApp::remoteEvent);
    remote.queueCommand("tracker", "ready");
    remote.sendCommands();
    
    //setup some variables, etc
	ofSetVerticalSync(true);
	cloneReady = false;
    
    //initiate the camera
    cam.setDeviceID(camID);
	cam.initGrabber(1280, 720);
    //and cloner, portrait mode
	clone.setup(drawWidth,drawHeight);
    
    //setup frame buffer objects
	ofFbo::Settings settings;
    
    //PORTRAIT ORIENTATION
	settings.width = cam.getHeight();
	settings.height = cam.getWidth();
    
    //to contain the white on black mask based on tracked face outline
	maskFbo.allocate(settings);
    maskImage.allocate(settings.width,settings.height,OF_IMAGE_COLOR_ALPHA);
    
    //to contain buffer to be mapped to face
    srcFbo.allocate(settings);
    //the rotated camera image
    camImage.allocate(settings.width,settings.height,OF_IMAGE_COLOR);

    //one more just for debug images
    if(debug){
        debugFbo.allocate(settings);
        debugFbo.begin();
        ofClear(0);
        debugFbo.end();
    }
       
    //check if temp directory has been generated
    remote.sendCommand("reqTmpDir", "now");
    
        
    
    //setup important objects
  	liveTracker.setup();
    
   // liveTracker.setIterations(12);

	basicTracker.setup();
	basicTracker.setIterations(25);
	basicTracker.setAttempts(4);
    tracking = false;
    
   
/* commented from original example which auto-loads directory of faces
	faces.allowExt("jpg");
	faces.allowExt("png");
	faces.listDir("faces");
	currentFace = 0;
	if(faces.size()!=0){
		loadFace(faces.getPath(currentFace));
	}
 */

}

void testApp::update() {
    remote.update();
    
	cam.update();
	if(cam.isFrameNew()) { //if camera frame is available
        
        //get camera image mirrored and rotated for portrait magic mirror.
        camImage = mirror( cam.getPixelsRef() );
        
        ofImage cvCamImage;
        cvCamImage.clone(camImage);
       
        /*
        if(scaleCV){
            cvCamImage.resize(cvCamImage.width/2, cvCamImage.height/2);
            //update tracker with new camera image
            liveTracker.update(toCv(cvCamImage.getPixelsRef()));

        }else{
            //update tracker with new camera image
            liveTracker.update(toCv(camImage.getPixelsRef()));
        }*/
        
        liveTracker.update(toCv(camImage.getPixelsRef()));
       		
		cloneReady = liveTracker.getFound();
        
        //if a face is found in the camera view
		if(cloneReady) {
            remote.queueCommand("face", "found");
            remote.sendCommands();
            
            if(debug){
                camMesh = liveTracker.getImageMesh();
                debugFbo.begin();
                ofClear(0,0);
                camMesh.drawWireframe();
                debugFbo.end();
            }
            
            if(tracking){ //If we are looking for faces.
                camMesh = liveTracker.getImageMesh();
                camMesh.clearTexCoords();
                camMesh.addTexCoords(srcPoints);  //srcPoints is empty unless a src face has been loaded
                
                
                //draw the white on black mask of the cam face
                maskFbo.begin();
                ofEnableAlphaBlending();
                ofClear(0, 0);
                camMesh.draw();
                maskFbo.end();
                
                //draw the mask again for the src.
                srcFbo.begin();
                ofClear(0, 0);
                extImage.bind();
                camMesh.draw();
                extImage.unbind();
                srcFbo.end();
                ofDisableAlphaBlending();
                
            
                
               
            }
            if(!imgSnapped) grabFace();  //if an img has not been grabbed, grab one of the tracked face.
            
            
			
			clone.setStrength(1);
            //finally, "Clone" the  src onto the cam texture reference, using a blurred mask.
			//clone.update(srcFbo.getTextureReference(), camImage.getTextureReference(), maskFbo.getTextureReference());
            clone.overlay(srcFbo.getTextureReference(), camImage.getTextureReference(), maskFbo.getTextureReference());

        }
	} 
}


void testApp::draw() {


    
	if(extImage.getWidth() > 0 && cloneReady && tracking) {
		clone.draw(0, 0);
        //camImage.draw(0, 0);
        //srcFbo.draw(0,0);
        
        
        
      
	} else {
       
		camImage.draw(0, 0, drawWidth, drawHeight);
      
	} 
	
    
    
    
    if(debug){
        ofEnableAlphaBlending();
        if(!liveTracker.getFound()) {
            drawHighlightString("camera face not found", 10, 10);
        }
        if(extImage.getWidth() == 0) {
            drawHighlightString("drag an image here", 10, 30);
        } else if(!basicTracker.getFound()) {
            drawHighlightString("image face not found", 10, 30);
        }
        drawHighlightString(dbString, 10, 40);
        drawDebug();
        ofDisableAlphaBlending();
    }
}

void testApp::drawDebug(){
    maskFbo.draw(camHeight, 0, debugWidth,debugHeight);
    drawHighlightString("maskFbo", camHeight+10,10);
    
    srcFbo.draw(camHeight, debugHeight,debugWidth,debugHeight);
    drawHighlightString("srcFBO", camHeight+10, debugHeight+10);
    
    debugFbo.draw(0,0);

 //   debugFbo.draw(camHeight, debugHeight*2,debugWidth,debugHeight);
    drawHighlightString("debug", camHeight+10, debugHeight*2+10);

    
}

//Load a src image for cloning onto the cam view
//Identify a face in the src and extract the texture coordinates (in pixels).

void testApp::loadFace(string face){
	extImage.loadImage(face);
	if(extImage.getWidth() > 0) {
	//	basicTracker.update(toCv(extImage));
	//	srcPoints = basicTracker.getImagePoints();
         tracking = true;
	}
}

void testApp::loadExt(string face){
	extImage.loadImage(face);
	if(extImage.getWidth() > 0) {
        	basicTracker.update(toCv(extImage));
        	srcPoints = basicTracker.getImagePoints();
        tracking = true;
	}
}



void testApp::dragEvent(ofDragInfo dragInfo) {
	loadExt(dragInfo.files[0]);
}



void testApp::grabFace(){
    imgSnapped = true;
    ofLog() << "Attempting to grab Source Image from webCam " << endl;
    tracking = false;
    basicTracker.update(toCv(camImage));
    
    Boolean faceFound = liveTracker.getFound();
    
    if(faceFound){
        
        srcPoints = basicTracker.getImagePoints();
        ofMesh trackedMesh = basicTracker.getImageMesh();
        faceRect = basicTracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE).getBoundingBox();
        time = ofGetTimestampString();
        camImage.saveImage(tmp+"/"+time+".png");
    
    
        ofPixels p;
        p.allocate(720,1280,OF_IMAGE_COLOR);
    
        maskFbo.begin();
        ofClear(0, 255);
        trackedMesh.draw();
        maskFbo.end();
    
        maskFbo.readToPixels(p);
    
        maskImage.setFromPixels(p);
        maskImage.reloadTexture();
        maskImage.saveImage(tmp+"/"+time+"mask.png");
    
   
    
    
        string cmd = "imageSaved";
        remote.queueCommand(cmd, time);
        cmd = "faceRect";
        string face = rect2str(faceRect);
        remote.queueCommand(cmd,face);
        extImage = camImage;
        
        remote.sendCommands();
    }else{
        imgSnapped = false;
    }
};

string testApp::rect2str(ofRectangle & rect){
    string ret;
    string x,y,wide,high,centerX,centerY;
    
    x = ofToString( ceil(rect.getX()) );
    y = ofToString( ceil(rect.getY()) );
    wide = ofToString( ceil(rect.getWidth()) );
    high = ofToString( ceil(rect.getHeight()) );
    centerX = ofToString( ceil(rect.getCenter().x) );
    centerY = ofToString( ceil(rect.getCenter().y) );
    
    ret=x+"x"+y+";"+wide+"x"+high+";"+centerX+"x"+centerY;
    
    return ret;
}

//IMAGE PROCESSES
ofImage testApp::mirror(ofPixels  p){
    ofImage i = p;
    i.rotate90(camRotation);
    i.mirror(1,0);
    return i;
}

//EVENTS
void testApp::keyPressed(int key){
    ofLog() << "key Pressed:" << key << endl;
	switch(key){
	case OF_KEY_UP:
		currentFace++;
		break;
	case OF_KEY_DOWN:
		currentFace--;
		break;
    case 's':
        grabFace();
        break;
    case 'x':
            imgSnapped = false;
	}
    
	currentFace = ofClamp(currentFace,0,faces.size());
	if(faces.size()!=0){
		loadFace(faces.getPath(currentFace));
	}
}

void testApp::remoteEvent(RemoteEvent &e){
    ofLog() << "Remote Event: "+e.command+","+ e.argument << endl;
    string com = e.command;
    if (com  == "snap"){
        remote.sendCommand("reqTmpDir", "now");
        imgSnapped = false;  //setup tracker to grab another image
    }else if(com  == "tmpDir"){
        tmp = e.argument;
        ofLogToFile(tmp+"clown.log");
        ofLog() << "tmp directory: "+tmp << endl;
    }else if(com == "facePainted"){
        loadFace(e.argument);
    }else if(com == "attract"){
        tracking = false;
    }
}

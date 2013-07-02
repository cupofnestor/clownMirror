

//
//  trackerRemote.h
//  FaceSubstitution
//
//  Created by W. Ryan Nestor on 5/7/13.
//
//
#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "RemoteEvent.h"

class TrackerRemote {
    public:
        //methods
        void update();
    
        void setup();
        void send(string m);
        void sendCommand(const string & command, const string & argument);
        void queueCommand(const string & command, const string & argument);
        void queueFace(const string & command, const string & argument);
        void sendCommands();
        //constructor
        TrackerRemote();

        //vars
        int sendPort = 12000;
        int recPort  = 11999;
        string commands;
    
        //constants
        const string COM_DEL = "[c]";
        const string ARG_DEL = "::";

    private:
        ofxUDPManager sendSocket;
        ofxUDPManager recSocket;
        void dispatch(string & m);
};


//
//  trackerRemote.cpp
//  FaceSubstitution
//
//  Created by W. Ryan Nestor on 5/7/13.
//
//


#include "trackerRemote.h"

TrackerRemote::TrackerRemote(){};

void TrackerRemote::setup(){
    recSocket.Create();
    recSocket.Bind(recPort);
    recSocket.SetNonBlocking(true);
    
    sendSocket.Create();
    sendSocket.Connect("127.0.0.1", sendPort);
    sendSocket.SetNonBlocking(true);
}

void TrackerRemote::update(){
    char udpMessage[1000];
    recSocket.Receive(udpMessage,1000);
    string m = udpMessage;
    if(m!=""){
        //cout << m << endl;
        dispatch(m);
        string m = "message";
        string a = "received";
        queueCommand(m,a);
        sendCommands();
       
    }
}

void TrackerRemote::send(string m){
    int sent = sendSocket.Send(m.c_str(),m.length());
    
}

void TrackerRemote::queueCommand(const string & command, const string & argument){
    commands.append(command+ARG_DEL+argument+COM_DEL);
}
void TrackerRemote::sendCommand(const string & command, const string & argument){
    commands.append(command+ARG_DEL+argument+COM_DEL);
    sendCommands();
}


void TrackerRemote::sendCommands(){
    send(commands);
    commands = "";
}

void TrackerRemote::dispatch(string & m){
    vector<string> commands = ofSplitString(m, "[c]");
    for (int n = 0; n<commands.size()-1; n++){
        string c = commands[n];
        static RemoteEvent ev;
        vector<string> result = ofSplitString(c,"::");
        ev.command = result[0];
        ev.argument = result[1];
        ofNotifyEvent(RemoteEvent::events, ev);

    }
    
}





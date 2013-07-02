//
//  RemoteEvent.h
//  FaceSubstitution
//
//  Created by W. Ryan Nestor on 5/7/13.
//
//

#ifndef __FaceSubstitution__RemoteEvent__
#define __FaceSubstitution__RemoteEvent__

#include <iostream>

#include "ofMain.h"
class RemoteEvent : public ofEventArgs {
    
public:
    
    string   command;
    string   argument;
    static ofEvent <RemoteEvent> events;
};


#endif /* defined(__FaceSubstitution__RemoteEvent__) */



//
//  msgRect.h
//  FaceSubstitution
//
//  Created by W. Ryan Nestor on 5/16/13.
//
//
#pragma once


#ifndef FaceSubstitution_msgRect_h
#define FaceSubstitution_msgRect_h
#include "ofMain.h"
#include <msgpack.hpp>

class msgRect
{
public:
    double x,y,wide,high;
    msgRect(){};
    msgRect( const ofRectangle & r){
        x = r.getX();
        y = y.getY();
        wide = r.getWidth();
        high = r.getHeight();
    }
    MSGPACK_DEFINE(x,y,wide,high,centerX,centerY);
};

#endif

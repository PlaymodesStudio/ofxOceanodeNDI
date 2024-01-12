//
//  ofxOceanodeNDI.h
//  example
//
//  Created by Eduard Frigola Bagué on 14/10/2020.
//

#ifndef ofxOceanodeNDI_h
#define ofxOceanodeNDI_h

#include "ofxOceanode.h"
#include "NDICamera.h"

namespace ofxOceanodeNDI{
static void registerModels(ofxOceanode &o){
    o.registerModel<NDICamera>("NDI");
}

static void registerType(ofxOceanode &o){
    
}

static void registerScope(ofxOceanode &o){
    
}

static void registerCollection(ofxOceanode &o){
    registerModels(o);
    registerType(o);
    registerScope(o);
}
}

#endif /* ofxOceanodeNDI_h */

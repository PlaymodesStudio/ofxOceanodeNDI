//
//  NDICamera.h
//  example
//
//  Created by Eduard Frigola on 30/11/23.
//

#ifndef NDICamera_h
#define NDICamera_h

#include "ofxOceanodeNodeModel.h"
#include "ofxNDI.h"

#include "ofxOceanodeTime.h"

class NDICamera: public ofxOceanodeNodeModel{
public:
    NDICamera();
    ~NDICamera();
    
    void setup() override;
    void update(ofEventArgs &a) override;
    
    void selectedDevice(int &identifier);
    
private:
    ofParameter<int> deviceID;
    ofParameter<ofTexture*> output;
    ofParameter<Timestamp> timestamp;
    
    ofTexture ndiTex1;
    ofTexture ndiTex2;
    ofTexture ndiTex3;
    
    ofxNDIreceiver ndiReceiver; // NDI receiver
    
    ofEventListeners listeners;
};

#endif /* NDICamera_h */

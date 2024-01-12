//
//  NDICamera.cpp
//  example
//
//  Created by Eduard Frigola on 30/11/23.
//

#include "NDICamera.h"

NDICamera::NDICamera() : ofxOceanodeNodeModel("NDI Camera"){
    
}

NDICamera::~NDICamera(){
    ndiReceiver.ReleaseReceiver();
}

void NDICamera::setup(){
    vector<string> videoDevices = {"None", "test"};
    
    addParameterDropdown(deviceID, "Device", 0, videoDevices);
    addParameter(output.set("Output", nullptr));
    addParameter(timestamp.set("Timestamp", 0));

    listeners.push(deviceID.newListener(this, &NDICamera::selectedDevice));
    
    color = ofColor::darkGreen;
}

void NDICamera::update(ofEventArgs &a){
    int64_t t1;
    int64_t t2;
    int64_t t3;
    
    int numframes = 0;
    if (ndiReceiver.ReceiveImage(ndiTex1)) {
        t1 = ndiReceiver.GetVideoTimestamp();
        timestamp = Timestamp(t1/10);
        output = &ndiTex1;
    }
    if (ndiReceiver.ReceiveImage(ndiTex2)) {
        t2 = ndiReceiver.GetVideoTimestamp();
        timestamp = Timestamp(t2/10);
        output = &ndiTex2;
    }
    if (ndiReceiver.ReceiveImage(ndiTex3)) {
        t3 = ndiReceiver.GetVideoTimestamp();
        timestamp = Timestamp(t3/10);
        output = &ndiTex3;
    }
}

void NDICamera::selectedDevice(int &identifier){
    if(identifier > 0){
        ndiReceiver.SetSenderIndex(identifier-1);
    }
}
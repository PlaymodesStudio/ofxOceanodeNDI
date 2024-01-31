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
    
    ndiTex1.allocate(1920, 1080, GL_RGBA);
    ndiTex2.allocate(1920, 1080, GL_RGBA);
    ndiTex2.allocate(1920, 1080, GL_RGBA);
    ndiReceiver.ReceiveImage(ndiTex1);
    ndiReceiver.ReceiveImage(ndiTex1);
    
    numSenders = ndiReceiver.GetSenderCount();
    
    videoDevices = {"None"};
    for(int i = 0; i < numSenders; i++){
        videoDevices.push_back(ndiReceiver.GetSenderName(i));
    }
    
    addParameterDropdown(deviceID, "Device", 0, videoDevices);
    addParameter(output.set("Output", nullptr));
    addParameter(timestamp.set("Timestamp", 0));

    listeners.push(deviceID.newListener(this, &NDICamera::selectedDevice));
    
    color = ofColor::darkGreen;
}

void NDICamera::update(ofEventArgs &a){
    ndiReceiver.FindSenders();
    if(ndiReceiver.GetSenderCount() != numSenders){
        numSenders = ndiReceiver.GetSenderCount();
        videoDevices = {"None"};
        for(int i = 0; i < numSenders; i++){
            videoDevices.push_back(ndiReceiver.GetSenderName(i));
        }
        
        auto it = std::find(videoDevices.begin(), videoDevices.end(), currentName);
        if(it != videoDevices.end()){
            deviceID = it - videoDevices.begin();
        }else{
            deviceID = 0;
        }
        
        getOceanodeParameter<int>(deviceID).setDropdownOptions(videoDevices);
        deviceID.setMax(videoDevices.size()-1);
    }
    
    int64_t t1;
    int64_t t2;
    int64_t t3;
    
    if(deviceID != 0){
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
}

void NDICamera::selectedDevice(int &identifier){
    if(identifier > 0){
        ndiReceiver.SetSenderIndex(identifier-1);
        currentName = ndiReceiver.GetSenderName(identifier-1);
    }else{
        currentName = "None";
    }
}

void NDICamera::presetSave(ofJson &json){
    json["ServerName"] = currentName;
}
    
void NDICamera::presetRecallAfterSettingParameters(ofJson &json){
    string name = "None";
    if(json.count("ServerName") == 1){
        name = json["ServerName"];
    }
    auto it = std::find(videoDevices.begin(), videoDevices.end(), name);
    
    if(it != videoDevices.end()){
        deviceID = it - videoDevices.begin();
    }else{
        deviceID = 0;
    }
}

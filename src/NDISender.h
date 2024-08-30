//
//  NDISender.h
//  example
//
//  Created by Eduard Frigola on 30/8/24.
//

#ifndef NDISender_h
#define NDISender_h

#include "ofxOceanodeNodeModel.h"
#include "ofxNDI.h"

class NDISender: public ofxOceanodeNodeModel{
public:
    NDISender() : ofxOceanodeNodeModel("NDI Sender"){};
    ~NDISender(){
        ndiSender.ReleaseSender();
    };
    
    void setup() override{
        addParameter(senderName.set("Name", "Texture"));
        
        listeners.push(senderName.newListener([this](string &s){
            ndiSender.ReleaseSender();
            ndiSender.CreateSender(senderName->c_str(), 100, 100);
        }));
        
        addParameter(input.set("Input", nullptr));
        
        ndiSender.SetReadback();
        ndiSender.SetAsync();
        ndiSender.CreateSender(senderName->c_str(), 100, 100);
    }
    
    void update(ofEventArgs &a) override{
        if(input.get() != nullptr){
            if(ndiSender.GetWidth() != input.get()->getWidth() || ndiSender.GetHeight() != input.get()->getHeight()){
                ndiSender.UpdateSender(input.get()->getWidth(), input.get()->getHeight());
            }
            ndiSender.SendImage(*input.get());
        }
    }
    
private:
    ofParameter<string> senderName;
    ofParameter<ofTexture*> input;

    ofxNDIsender ndiSender; // NDI receiver
    ofEventListeners listeners;
};


#endif /* NDISender_h */

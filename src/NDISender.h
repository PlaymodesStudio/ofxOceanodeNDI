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
            createNDISender();
        }));
        
        addParameter(input.set("Input", nullptr));
        createNDISender();
    }
    void createNDISender()
    {
        // Option : set readback
        // Pixel data extraction from fbo or texture
        // is optimised using two OpenGL pixel buffers (pbo's)
        // Note that the speed can vary with different CPUs
        //ndiSender.SetReadback();
        
        // Option : set NDI asynchronous sending
        // If disabled, the render rate is clocked to the sending framerate.
        //ndiSender.SetAsync();
        
        ndiSender.SetFrameRate(60);
        
        int w,h = 100;
        if(input.get()!=NULL)
        {
            w = input.get()->getWidth();
            h = input.get()->getHeight();
        }
        
        ndiSender.CreateSender(senderName->c_str(), w, h);
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

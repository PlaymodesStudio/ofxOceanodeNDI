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
		addParameter(input.set("Input", nullptr));
		addParameter(senderName.set("Name", "Texture"));
		addParameterDropdown(format, "NDI Format", 9,
							 {"UYVY", "UYVA", "P216", "PA16", "YV12", "I420", "NV12",
			"BGRA", "BGRX", "RGBA", "RGBX"});
		addParameterDropdown(convertTo, "Convert To", 0, {"None", "GL_RGB8", "GL_RGBA8", "GL_RGB16", "GL_RGBA16", "GL_RGB16F", "GL_RGBA16F", "GL_RGB32F", "GL_RGBA32F"});
		addParameter(readback.set("Readback",false));
		addParameter(async.set("Async",false));
		
		//···································································//
		inputListener = input.newListener([this](ofTexture* &tex){
			if(input.get() != nullptr)
			{
				if((input.get()->getWidth() != conversionFbo.getWidth()) || (input.get()->getHeight() != conversionFbo.getHeight()))
				{
					allocateConversionFBO(convertTo);
				}
			}			
		});

		//···································································//
		serverNameListener = senderName.newListener([this](string &s){
			ndiSender.ReleaseSender();
			createNDISender();
		});
		
		//···································································//
		readbackListener = readback.newListener([this](bool &b){
			ndiSender.SetReadback(b);
		});
		
		//···································································//
		asyncListener = async.newListener([this](bool &b){
			ndiSender.SetAsync(b);
		});
		
		//···································································//
		formatListener = format.newListener([this](int &i){
			// changed format
			switch(i)
			{
				case 0:
					ndiSender.SetFormat(NDIlib_FourCC_video_type_UYVY);
					break;
				case 1:
					ndiSender.SetFormat(NDIlib_FourCC_video_type_UYVA);
					break;
				case 2:
					ndiSender.SetFormat(NDIlib_FourCC_video_type_P216);
					break;
				case 3:
					ndiSender.SetFormat(NDIlib_FourCC_video_type_PA16);
					break;
				case 4:
					ndiSender.SetFormat(NDIlib_FourCC_video_type_YV12);
					break;
				case 5:
					ndiSender.SetFormat(NDIlib_FourCC_video_type_I420);
					break;
				case 6:
					ndiSender.SetFormat(NDIlib_FourCC_video_type_NV12);
					break;
				case 7:
					ndiSender.SetFormat(NDIlib_FourCC_video_type_BGRA);
					break;
				case 8:
					ndiSender.SetFormat(NDIlib_FourCC_video_type_BGRX);
					break;
				case 9:
					ndiSender.SetFormat(NDIlib_FourCC_video_type_RGBA);
					break;
				case 10:
					ndiSender.SetFormat(NDIlib_FourCC_video_type_RGBX);
					break;
					
				default:
					ndiSender.SetFormat(NDIlib_FourCC_video_type_RGBA);
					break;
			}
		});
		
		//···································································//
		convertToListener = convertTo.newListener([this](int &i)
		{
			if(i == 0) {
				// None - no conversion needed
				return;
			}
			// Allocate FBO with the selected format if input texture exists
			allocateConversionFBO(i);
		});
		//···································································//

		ofLog() << "NDI version - " << ndiSender.GetNDIversion() << endl;
		const GLubyte* ver = glGetString(GL_SHADING_LANGUAGE_VERSION);
		if(ver){
			ofLogNotice() << "GLSL version string: " << (const char*)ver;
		}else{
			ofLogError() << "Could not query GLSL version";
		}
		
		allocateConversionFBO(convertTo);
		createNDISender();
	}
	
	//-----------------------------------------------------------------//

	void createNDISender()
	{
		ndiSender.SetFrameRate(60);
		
		int w = 100;
		int h = 100;
		
		if(input.get()!=NULL)
		{
			w = input.get()->getWidth();
			h = input.get()->getHeight();
		}
		
		NDIInitialized = ndiSender.CreateSender(senderName->c_str(), w, h);
		
		if (!NDIInitialized) {
			std::string str = "Could not create sender [";
			str += senderName;	str += "]";
			ofLog() << "Could not create %s\n" << str.c_str() << endl;
			senderName += "_";
			NDIInitialized = ndiSender.CreateSender(senderName->c_str(), w, h);
			// If that still fails warn the user and quit
			if (!NDIInitialized) {
				ofSystemAlertDialog(str);
				//exit();
			}
		}
		ofLog() << "Created sender : " << senderName->c_str() << " Format : " << ndiSender.GetFormat() << endl;
	}
	
	void update(ofEventArgs &a) override{
		if(input.get() != nullptr){
			// This check is already one in ndiSender.sendImage() ?
			//            if(ndiSender.GetWidth() != input.get()->getWidth() || ndiSender.GetHeight() != input.get()->getHeight())
			//			{
			//                ndiSender.UpdateSender(input.get()->getWidth(), input.get()->getHeight());
			//            }
			
			if(convertTo == 0) {
				// No conversion - send texture directly
				ndiSender.SendImage(*input.get());
			} else {
				// Convert texture format using FBO
				
				if(conversionFbo.isAllocated()) {
					conversionFbo.begin();
					ofClear(0);
					input.get()->draw(0, 0, conversionFbo.getWidth(), conversionFbo.getHeight());
					conversionFbo.end();
					ndiSender.SendImage(conversionFbo.getTexture());
				} else {
					// FBO not allocated, send directly as fallback
					ndiSender.SendImage(*input.get());
				}
			}
		}
	}
		
	void allocateConversionFBO(int i)
	{
		// Map index to GL internal format
		int glInternalFormat;
		switch(i) {
			case 1: glInternalFormat = GL_RGB8; break;
			case 2: glInternalFormat = GL_RGBA8; break;
			case 3: glInternalFormat = GL_RGB16; break;
			case 4: glInternalFormat = GL_RGBA16; break;
			case 5: glInternalFormat = GL_RGB16F; break;
			case 6: glInternalFormat = GL_RGBA16F; break;
			case 7: glInternalFormat = GL_RGB32F; break;
			case 8: glInternalFormat = GL_RGBA32F; break;
			default: return;
		}

		// Allocate FBO with the selected format if input texture exists
		if(input.get() != nullptr) {
			int w = input.get()->getWidth();
			int h = input.get()->getHeight();
			if(w > 0 && h > 0) {
				ofFboSettings settings;
				settings.width = w;
				settings.height = h;
				settings.internalformat = glInternalFormat;
				conversionFbo.allocate(settings);
			}
		}
	}

	
private:
	ofParameter<string> senderName;
	ofParameter<ofTexture*> input;
	ofParameter<int> format;
	ofParameter<bool> readback;
	ofParameter<bool> async;
	ofParameter<int> convertTo;
	
	
	bool NDIInitialized = false;
	ofxNDIsender ndiSender; // NDI receiver
	ofFbo conversionFbo;
	
	ofEventListener inputListener;
	ofEventListener serverNameListener;
	ofEventListener formatListener;
	ofEventListener readbackListener;
	ofEventListener asyncListener;
	ofEventListener convertToListener;
};


#endif /* NDISender_h */

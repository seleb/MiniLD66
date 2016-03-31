#pragma once

#include <MY_Scene_Base.h>

class StereoCamera;

class MY_Scene_VR : public MY_Scene_Base{
public:
	// To render a VR scene, we need a stereo camera
	// Stereo cameras behave like normal perspective cameras, except they also have two child cameras (one for each eye)
	// If an Oculus Rift is connected at start-up, the stereo camera will construct all the necessary resources for rendering using the Odculus SDK and will adjust based on HMD data
	// If an Oculus Rift isn't connected at start-up, the stereo camera will simply render normally
	StereoCamera * vrCam;
	// shader for cubes
	ComponentShaderBase * diffuseShader;

	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	MY_Scene_VR(Game * _game);
	~MY_Scene_VR();
};
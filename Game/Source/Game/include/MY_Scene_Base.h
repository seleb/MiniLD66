#pragma once

#include <Scene.h>
#include <Game.h>
#include <UILayer.h>
#include <MousePerspectiveCamera.h>

#include <MY_ResourceManager.h>
#include <sweet/Input.h>

class ComponentShaderText;
class Font;


// This scene class acts as a base for our other scenes
// Typically, we're going to have at least some common or shared code
// which isn't a part of the default scene class, so we can put it all here
class MY_Scene_Base : public Scene{
public:
	// The main shader for the scene.
	// This typically contains at least an MVP component, and a texture component.
	// It usually also has a lighting component of some sort (diffuse, phong, toon, etc).
	ComponentShaderBase * baseShader;
	// In order to render UI text (e.g. the type in a TextLabel or a TextArea)
	// we need a text shader. We create one in the base scene even though we aren't using it
	// so that we don't meed to make one in any derived scenes that need it.
	ComponentShaderText * textShader;
	// In order to render UI text (e.g. the type in a TextLabel or a TextArea)
	// we need a font. We create one in the base scene even though we aren't using it
	// so that we don't meed to make one in any derived scenes that need it.
	Font * font;
	
	// We're going to include a MousePerspectiveCamera (a standard PerspectiveCamera with a built-in controller object)
	// in the base scene in order to provide us with something to look through no matter what
	MousePerspectiveCamera * debugCam;

	// UILayers are essentially treated as 2D canvases which can be rendered
	// on top of the rest of the scene
	// Typically, we resize them during each update in order to keep them proportional
	// to the screen's dimensions
	// They are derived from NodeUI, so we can use the NodeUI layout system instead
	// of manually positioning other NodeUI elements during the update loop
	UILayer * const uiLayer;

	// The joystick class 
	Joystick * const controller;

	MY_Scene_Base(Game * _game);
	virtual ~MY_Scene_Base();

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	
	virtual void load() override;
	virtual void unload() override;


	// Toggles the debug mode
	void toggleDebug();
	// The visibility of transform indicators is used as a flag in order to determine whether debug mode is enabled
	bool isDebugEnabled();
	
	// Enables the debug mode
	// By default, this only makes the transform indicators visible in the scene and turns on debugging for the uiLayer
	// This should be extended in derived scenes in order to add extra debug features
	// e.g. physics debug drawing
	virtual void enableDebug();
	// Disables the debug mode
	// By default, this only makes the transform indicators invisible in the scene and turns off debugging for the uiLayer
	// This should be extended in derived scenes in order to remove extra debug features
	// e.g. physics debug drawing
	virtual void disableDebug();
};
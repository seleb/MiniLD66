#pragma once

#include <MY_Scene_Base.h>

#include <MeshEntity.h>
#include <MeshInterface.h>
#include <MeshFactory.h>

#include <shader\ComponentShaderBase.h>
#include <shader\ComponentShaderText.h>
#include <shader\ShaderComponentText.h>
#include <shader\ShaderComponentTexture.h>
#include <shader\ShaderComponentDiffuse.h>
#include <shader\ShaderComponentMVP.h>

#include <NumberUtils.h>

#include <RenderOptions.h>

MY_Scene_Base::MY_Scene_Base(Game * _game) :
	Scene(_game),
	baseShader(new ComponentShaderBase(true)),
	textShader(new ComponentShaderText(true)),
	font(MY_ResourceManager::globalAssets->getFont("FONT")->font),
	uiLayer(new UILayer(0,0,0,0)),
	debugCam(new MousePerspectiveCamera()),
	controller(new JoystickVirtual(0))
{
	baseShader->addComponent(new ShaderComponentMVP(baseShader));
	//baseShader->addComponent(new ShaderComponentDiffuse(baseShader));
	baseShader->addComponent(new ShaderComponentTexture(baseShader));
	baseShader->compileShader();
	baseShader->name = "MY_Scene_Base base shader";

	textShader->textComponent->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.f));
	textShader->name = "MY_Scene_Base text shader";

	// Set up debug camera
	cameras.push_back(debugCam); // Add it to the cameras list (this isn't strictly necessary, but is useful organizational and debugging purposes)
	childTransform->addChild(debugCam); // Add it to the scene to give it a position and remove the need to update it manually
	activeCamera = debugCam; // Make it the active camera so that it is the source for the view-projection matrices
	// configure the camera's default properties
	debugCam->farClip = 1000.f;
	debugCam->nearClip = 0.01f;
	debugCam->firstParent()->translate(5.0f, 1.5f, 22.5f);
	debugCam->yaw = 90.0f;
	debugCam->pitch = -10.0f;
	

	// reference counting for member variables
	baseShader->incrementReferenceCount();
	textShader->incrementReferenceCount();
}

MY_Scene_Base::~MY_Scene_Base(){
	deleteChildTransform();

	// auto-delete member variables
	baseShader->decrementAndDelete();
	textShader->decrementAndDelete();

	delete uiLayer;
	delete controller;
}


void MY_Scene_Base::update(Step * _step){
	controller->update(_step);

	// basic debugging controls
	if(keyboard->keyJustDown(GLFW_KEY_ESCAPE)){
		// if the user hits escape on the menu, exit the game
		// if the user hits escape anywhere else, take them to the menu
		if(game->scenes["menu"] != this){
			game->switchScene("menu", false);
		}
	}if(keyboard->keyJustDown(GLFW_KEY_F11)){
		game->toggleFullScreen();
	}if(keyboard->keyJustDown(GLFW_KEY_1)){
		cycleCamera();
	}if(keyboard->keyJustDown(GLFW_KEY_2)){
		toggleDebug();
	}

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0, sd.x, 0, sd.y);
	Scene::update(_step);
	uiLayer->update(_step);
}

void MY_Scene_Base::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer->render(_matrixStack, _renderOptions);
}

void MY_Scene_Base::load(){
	Scene::load();	
	uiLayer->load();
}

void MY_Scene_Base::unload(){
	uiLayer->unload();
	Scene::unload();	
}


bool MY_Scene_Base::isDebugEnabled(){	
	return Transform::drawTransforms;
}

void MY_Scene_Base::toggleDebug(){
	isDebugEnabled() ? disableDebug() : enableDebug();
}

void MY_Scene_Base::enableDebug(){
	Transform::drawTransforms = true;
	uiLayer->bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
}
void MY_Scene_Base::disableDebug(){
	Transform::drawTransforms = false;
	uiLayer->bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);
}
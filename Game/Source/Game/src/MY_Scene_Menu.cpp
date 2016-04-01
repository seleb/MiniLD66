#pragma once

#include <MY_Scene_Menu.h>
#include <MY_Scene_Main.h>
#include <shader/ComponentShaderText.h>
#include <sweet/UI.h>
#include <FileUtils.h>
#include <RenderSurface.h>
#include <StandardFrameBuffer.h>
#include <RenderOptions.h>

MY_Scene_Menu::MY_Scene_Menu(Game * _game) :
	MY_Scene_Base(_game),
	screenSurfaceShader(new Shader("assets/RenderSurface_1", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader, true)),
	screenFBO(new StandardFrameBuffer(true)),
	colTime(0)
{
	// memory management
	screenSurface->incrementReferenceCount();
	screenSurfaceShader->incrementReferenceCount();
	screenFBO->incrementReferenceCount();


	NodeUI * layout = new NodeUI(uiLayer->world);
	uiLayer->addChild(layout, false);
	layout->setRationalHeight(1.f, uiLayer);
	layout->setRationalWidth(1.f, uiLayer);
	layout->setRenderMode(kTEXTURE);
	layout->setBackgroundColour(0,0,0);
	layout->background->setVisible(false);

	// Create some text labels
	textShader->setColor(1,1,1);
	TextArea * readMe = new TextArea(uiLayer->world, MY_ResourceManager::globalAssets->getFont("FONT-SMALL")->font, textShader);
	layout->addChild(readMe, false);
	readMe->setRationalWidth(1.f, layout);
	readMe->setRationalHeight(1.f, layout);
	readMe->verticalAlignment = kMIDDLE;
	readMe->setWrapMode(WrapMode::kWORD);
	readMe->setMargin(0.05f, 0.1f);

	// set the text on the labels
	readMe->setText(sweet::FileUtils::readFile("readme.txt"));

	uiLayer->invalidateLayout();
	uiLayer->addMouseIndicator();
}

void MY_Scene_Menu::update(Step * _step){

	float r = MY_Scene_Main::SIZE/2;
	colTime += _step->deltaTime*0.25f;
	

	if(keyboard->keyJustDown(GLFW_KEY_ENTER)){
		game->scenes["main"] = new MY_Scene_Main(game);
		game->switchScene("main", false);
	}
	MY_Scene_Base::update(_step);

	screenSurfaceShader->bindShader(); // remember that we have to bind the shader before it can be updated
	GLint test = glGetUniformLocation(screenSurfaceShader->getProgramId(), "time");
	checkForGlError(0);
	if(test != -1){
		glUniform1f(test, _step->time);
		checkForGlError(0);
	}
}

MY_Scene_Menu::~MY_Scene_Menu(){	
	// memory management
	screenSurface->decrementAndDelete();
	screenSurfaceShader->decrementAndDelete();
	screenFBO->decrementAndDelete();
}

void MY_Scene_Menu::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	textShader->setColor(glm::min(1.f, glm::sin(colTime) + 1.f), (glm::sin(colTime+15)+1)*0.5f, (glm::sin(colTime*2 + 15)+1)*0.4f+0.1f, 1.f);
	glm::vec4 v = textShader->getColor()*0.25f;
	_renderOptions->setClearColour(v.x, v.y, v.z, 1);
	// keep our screen framebuffer up-to-date with the current viewport
	screenFBO->resize(_renderOptions->viewPortDimensions.width, _renderOptions->viewPortDimensions.height);

	// bind our screen framebuffer
	FrameBufferInterface::pushFbo(screenFBO);
	// render the scene
	//uiLayer->setVisible(false);
	MY_Scene_Base::render(_matrixStack, _renderOptions);
	// unbind our screen framebuffer, rebinding the previously bound framebuffer
	// since we didn't have one bound before, this will be the default framebuffer (i.e. the one visible to the player)
	FrameBufferInterface::popFbo();

	// render our screen framebuffer using the standard render surface
	screenSurface->render(screenFBO->getTextureId());

	// render the uiLayer after the screen surface in order to avoid hiding it through shader code
	//uiLayer->setVisible(true);
	//uiLayer->render(_matrixStack, _renderOptions);
}
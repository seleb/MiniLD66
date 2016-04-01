#pragma once

#include <MY_Scene_Menu.h>
#include <MY_Scene_Main.h>
#include <shader/ComponentShaderText.h>
#include <sweet/UI.h>
#include <FileUtils.h>
MY_Scene_Menu::MY_Scene_Menu(Game * _game) :
	MY_Scene_Base(_game)
{
	NodeUI * layout = new NodeUI(uiLayer->world);
	uiLayer->addChild(layout, false);
	layout->setRationalHeight(1.f, uiLayer);
	layout->setRationalWidth(1.f, uiLayer);
	layout->setRenderMode(kTEXTURE);
	layout->setBackgroundColour(0,0,0);

	// Create some text labels
	textShader->setColor(1,1,1);
	TextArea * readMe = new TextArea(uiLayer->world, font, textShader);
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
	if(keyboard->keyJustDown(GLFW_KEY_ENTER)){
		game->scenes["main"] = new MY_Scene_Main(game);
		game->switchScene("main", false);
	}
	MY_Scene_Base::update(_step);
}
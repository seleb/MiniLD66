#pragma once

#include <MY_Scene_Menu.h>
#include <shader/ComponentShaderText.h>
#include <sweet/UI.h>

MY_Scene_Menu::MY_Scene_Menu(Game * _game) :
	MY_Scene_Base(_game)
{
	// Create a linear layout to contain all of our menu items
	// remember that these elements are all going to exist in the uiLayer, so we pass in its physics world in the constructors
	VerticalLinearLayout * layout = new VerticalLinearLayout(uiLayer->world);
	layout->horizontalAlignment = kCENTER;
	layout->verticalAlignment = kMIDDLE;
	// set the layout's size to 100% of its parent
	layout->setRationalHeight(1.f, uiLayer);
	layout->setRationalWidth(1.f, uiLayer);

	// Create some text labels
	TextLabel * label1 = new TextLabel(uiLayer->world, font, textShader);
	TextLabel * label2 = new TextLabel(uiLayer->world, font, textShader);
	TextLabel * label3 = new TextLabel(uiLayer->world, font, textShader);
	TextLabel * label4 = new TextLabel(uiLayer->world, font, textShader);
	TextLabel * label5 = new TextLabel(uiLayer->world, font, textShader);
	TextLabel * label6 = new TextLabel(uiLayer->world, font, textShader);

	// set the text on the labels
	label1->setText("Box2D Test Scene");
	label2->setText("Bullet3D Test Scene");
	label3->setText("Surface Shader Test Scene");
	label4->setText("Screen Shader Test Scene");
	label5->setText("VR Test Scene");
	label6->setText("Quit Game");

	// make the labels' background visible (by default both the scene's clear colour and the text colour will be black)
	label1->setBackgroundColour(1,1,1,1);
	label2->setBackgroundColour(1,1,1,1);
	label3->setBackgroundColour(1,1,1,1);
	label4->setBackgroundColour(1,1,1,1);
	label5->setBackgroundColour(1,1,1,1);
	label6->setBackgroundColour(1,1,1,1);

	// make the labels clickable
	label1->setMouseEnabled(true);
	label2->setMouseEnabled(true);
	label3->setMouseEnabled(true);
	label4->setMouseEnabled(true);
	label5->setMouseEnabled(true);
	label6->setMouseEnabled(true);

	// add listeners to each label, making them buttons that take the player to different scenes
	label1->eventManager->addEventListener("click", [&](sweet::Event * _event){
		game->switchScene("box2d", false);
	});
	label2->eventManager->addEventListener("click", [&](sweet::Event * _event){
		game->switchScene("bullet3d", false);
	});
	label3->eventManager->addEventListener("click", [&](sweet::Event * _event){
		game->switchScene("surfaceshaders", false);
	});
	label4->eventManager->addEventListener("click", [&](sweet::Event * _event){
		game->switchScene("screenshaders", false);
	});
	label5->eventManager->addEventListener("click", [&](sweet::Event * _event){
		game->switchScene("vr", false);
	});
	label6->eventManager->addEventListener("click", [&](sweet::Event * _event){
		game->exit();
	});


	// add the labels to the layout
	layout->addChild(label1);
	layout->addChild(label2);
	layout->addChild(label3);
	layout->addChild(label4);
	layout->addChild(label5);
	layout->addChild(label6);

	// add the layout to the uiLayer
	uiLayer->addChild(layout);

	uiLayer->invalidateLayout();

	// add a mouse indicator (AKA a cursor) to the uiLayer so that the user can see what they're doing
	uiLayer->addMouseIndicator();
}
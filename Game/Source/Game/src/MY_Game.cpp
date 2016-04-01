#pragma once

#include <MY_Game.h>

#include <MY_ResourceManager.h>

#include <MY_Scene_Menu.h>
#include <MY_Scene_Box2D.h>
#include <MY_Scene_Bullet3D.h>
#include <MY_Scene_SurfaceShaders.h>
#include <MY_Scene_ScreenShaders.h>
#include <MY_Scene_VR.h>
#include <MY_Scene_Main.h>


MY_Game::MY_Game() :
	Game("menu", new MY_Scene_Menu(this), true) // initialize our game with a menu scene
{
}

MY_Game::~MY_Game(){}

void MY_Game::addSplashes(){
	// add default splashes
	Game::addSplashes();

	// add custom splashes
	addSplash(new Scene_Splash(this, MY_ResourceManager::globalAssets->getTexture("DEFAULT")->texture, MY_ResourceManager::globalAssets->getAudio("DEFAULT")->sound));
}
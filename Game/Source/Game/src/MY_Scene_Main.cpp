#pragma once

#include <MY_Scene_Main.h>


#include <MeshFactory.h>

#include <DirectionalLight.h>
#include <NumberUtils.h>

MY_Scene_Main::MY_Scene_Main(Game * _game) :
	MY_Scene_Base(_game),
{
}

MY_Scene_Main::~MY_Scene_Main(){	
}

void MY_Scene_Main::update(Step * _step){
	MY_Scene_Base::update(_step);
}

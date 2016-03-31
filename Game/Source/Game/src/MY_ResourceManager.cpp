#pragma once

#include <MY_ResourceManager.h>

Scenario * MY_ResourceManager::globalAssets = nullptr;

MY_ResourceManager::MY_ResourceManager(){
	globalAssets = new Scenario("assets/scenario.json");
	resources.push_back(globalAssets);

	load();
}

MY_ResourceManager::~MY_ResourceManager(){
	unload();
	destruct();
}
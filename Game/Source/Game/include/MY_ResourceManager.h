#pragma once

#include <ResourceManager.h>
#include <scenario\Scenario.h>

class MY_ResourceManager : public ResourceManager{
public:
	// A container for all of the assets which are loaded at initialization and are accessible from anywhere in the application, at any time
	static Scenario * globalAssets;
	
	MY_ResourceManager();
	~MY_ResourceManager();
};